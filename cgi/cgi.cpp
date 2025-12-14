#include "cgi.hpp"


std::string CGI::execute()
{
    int pipeIn[2];
    int pipeOut[2];

    if(pipe(pipeIn) == -1)
    {
        std::cerr << "Error: Failed to create input pipe" << std::endl;
		return "HTTP/1.0 500 Internal Server Error\r\n\r\nPipe creation failed";
    }

    if (pipe(pipeOut) == -1)
    {
        close(pipeIn[0]);
        close(pipeIn[1]);
        std::cerr << "Error: Failed to create output pipe" << std::endl;
		return "HTTP/1.0 500 Internal Server Error\r\n\r\nPipe creation failed";
    }

    pid_t pid = fork();

    if(pid == -1)
    {
        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);
        std::cerr << "Error: Fork failed" << std::endl;
		return "HTTP/1.0 500 Internal Server Error\r\n\r\nFork failed";
    }

    if (pid == 0)
    {
        //child read from pipeIn => close write end
        close(pipeIn[1]);
        //child write from pipeOut => close read end
        close(pipeOut[0]);

        if (dup2(pipeIn[0], STDIN_FILENO) == -1)
        {
            std::cerr << "Error: dup2 stdin failed" << std::endl;
            std::exit(1);
        }

        if (dup2(pipeOut[1], STDOUT_FILENO) == -1)
        {
            std::cerr << "Error: dup2 stdout faild" << std::endl;
            std::exit(1);
        }

        close(pipeIn[0]);
        close(pipeOut[1]);

        if (!_workingDir.empty())
        {
            if(chdir(_workingDir.c_str()) == -1)
            {
                std::cerr << "Error: chdir faild: " << _workingDir << std::endl;
                std::exit(1);
            }
        }


        std::map<std::string, std::string> envMap = setupEnvironment();
        char** envp = mapToEnvArray(envMap);

        std::string interpreter = getCGIInterpreter(_scriptPath);

        if (!interpreter.empty())
		{
			char* argv[3];
			argv[0] = const_cast<char*>(interpreter.c_str());
			argv[1] = const_cast<char*>(_scriptPath.c_str());
			argv[2] = NULL;
			
			execve(interpreter.c_str(), argv, envp);
		}
		else
		{
			char* argv[2];
			argv[0] = const_cast<char*>(_scriptPath.c_str());
			argv[1] = NULL;
			
			execve(_scriptPath.c_str(), argv, envp);
		}

		freeEnvArray(envp);
		std::cerr << "Error: execve failed for " << _scriptPath << std::endl;
		std::exit(1);
    }
    else
    {
        close(pipeIn[0]);
        // Parent will READ from pipeOut => close write end
        close(pipeOut[1]);

        /*
        ** Send request body to CGI (for POST requests)
        **
        ** Example: POST data like "username=john&password=123"
        ** CGI script reads this from stdin
        */
        if (!_body.empty())
        {
            ssize_t written = write(pipeIn[1], _body.c_str(), _body.length());
            if (written == -1)
            {
                std::cerr << "Error: Failed to write body to CGI" << std::endl;
            }
        }
        
        /*
        ** IMPORTANT: Close write end of pipe
        ** CGI expects EOF (end of file) to know request is complete
        ** If we don't close, CGI will wait forever for more data!
        */
        close(pipeIn[1]);

        /*
        ** Read CGI output
        **
        ** We need to read everything the CGI script outputs
        ** But we also need timeout protection (script might hang)
        */
        
        // Set pipe to non-blocking mode (allows timeout check)
        int flags = fcntl(pipeOut[0], F_GETFL, 0);
        fcntl(pipeOut[0], F_SETFL, flags | O_NONBLOCK);
        
        std::string output;
        char buffer[4096];
        time_t startTime = time(NULL);
        
        while (true)
        {
            ssize_t bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1);
            
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                output.append(buffer, bytesRead);
            }
            else if (bytesRead == 0)
            {
                break;
            }
            else
            {
                if (time(NULL) - startTime > _timeout)
                {
                    std::cerr << "Error: CGI timeout" << std::endl;
                    kill(pid, SIGKILL);
                    close(pipeOut[0]);
                    waitpid(pid, NULL, 0);
                    return "HTTP/1.0 504 Gateway Timeout\r\n\r\nCGI execution timeout";
                }
                
                // No data yet, sleep a bit before trying again
                usleep(10000);
            }
        }
        
        close(pipeOut[0]);

        /*
        ** A zombie is a dead process that wasn't cleaned up by parent
        ** 1. Waits for child to finish (if not finished)
        ** 2. Cleans up child's process table entry
        */
        int status;
        waitpid(pid, &status, 0);

        /*
        ** Check if child exited normally
        ** WIFEXITED(status): Returns true if child exited normally
        ** WEXITSTATUS(status): Returns exit code
        */
        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);
            if (exitCode != 0)
            {
                std::cerr << "Warning: CGI exited with code " << exitCode << std::endl;
            }
        }
        else if (WIFSIGNALED(status))
        {
            // Child was killed by signal
            std::cerr << "Error: CGI was killed by signal" << std::endl;
            return "HTTP/1.0 500 Internal Server Error\r\n\r\nCGI execution failed";
        }

        // Format and return response
        return formatCGIOutput(output);
    }

}