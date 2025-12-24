#include "cgi.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

CGI::CGI() 
	: _pid(-1), _outFile(-1), _startTime(0)
{
}

CGI::~CGI()
{
	if (_outFile != -1)
	{
		close(_outFile);
		_outFile = -1;
	}
	if (!_inputFile.empty())
	{
		unlink(_inputFile.c_str());
		_inputFile.clear();
	}
	if (!_outputFile.empty())
	{
		unlink(_outputFile.c_str());
		_outputFile.clear();
	}
	if (_pid > 0)
	{
		kill(_pid, SIGKILL);
		waitpid(_pid, NULL, WNOHANG);
		_pid = -1;
	}
}

void CGI::setScriptPath(const std::string& path)
{
	_scriptPath = path;
}

void CGI::setMethod(const std::string& method)
{
	_method = method;
}

void CGI::setQueryString(const std::string& query)
{
	_queryString = query;
}

void CGI::setInputFile(const std::string& filePath)
{
	_inputFile = filePath;
}

void CGI::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

pid_t CGI::getPid() const
{
	return _pid;
}

int CGI::getOutFile() const
{
	return _outFile;
}

time_t CGI::getStartTime() const
{
	return _startTime;
}

const std::string& CGI::getOutput() const
{
	return _output;
}

std::string CGI::getCGIInterpreter(const std::string& path)
{
	size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
		return "";
	std::string ext = path.substr(dotPos);
	if (ext == ".php")
		return "/usr/bin/php-cgi";
	else if (ext == ".py")
		return "/usr/bin/python3";
	return "";
}

std::map<std::string, std::string> CGI::setupEnvironment()
{
	std::map<std::string, std::string> env;
	env["REQUEST_METHOD"] = _method;
	env["SCRIPT_FILENAME"] = _scriptPath;
	env["QUERY_STRING"] = _queryString;
	env["SERVER_PROTOCOL"] = "HTTP/1.0";
	env["GATEWAY_INTERFACE"] = "CGI/1.0";
	env["REDIRECT_STATUS"] = "200";
	std::ostringstream oss;
	oss << _body.length();
	env["CONTENT_LENGTH"] = oss.str();
	std::map<std::string, std::string>::iterator it = _headers.find("Content-Type");
	if (it != _headers.end())
		env["CONTENT_TYPE"] = it->second;
	// Convert HTTP headers to HTTP_* variables
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::string key = "HTTP_" + it->first;
		for (size_t i = 0; i < key.length(); i++)
		{
			if (key[i] == '-')
				key[i] = '_';
			else if (key[i] >= 'a' && key[i] <= 'z')
				key[i] = key[i] - 32;
		}
		env[key] = it->second;
	}
	return env;
}

char** CGI::mapToEnvArray(const std::map<std::string, std::string>& env)
{
	char** envp = new char*[env.size() + 1];
	size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it = env.begin();
		 it != env.end(); ++it)
	{
		std::string envVar = it->first + "=" + it->second;
		envp[i] = new char[envVar.length() + 1];
		std::strcpy(envp[i], envVar.c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

void CGI::freeEnvArray(char** envp)
{
	if (!envp)
		return;
	for (size_t i = 0; envp[i] != NULL; i++)
		delete[] envp[i];
	delete[] envp;
}

bool CGI::start()
{
	std::ostringstream outputPath;
	outputPath << "/tmp/cgi_output_" << time(NULL) << ".html";
	_outputFile = outputPath.str();

	int outputFd = open(_outputFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (outputFd < 0)
	{
		std::cerr << "Failed to create output file: " << _outputFile << std::endl;
		return false;
    }
	close(outputFd);

	_pid = fork();

	if (_pid < 0)
	{
		std::cerr << "Fork failed\n";
		unlink(_outputFile.c_str());
		return false;
	}
	if (_pid == 0)
	{
		if (!_inputFile.empty())
		{
			int inFd = open(_inputFile.c_str(), O_RDONLY);
			if (inFd < 0)
			{
				std::cerr << "Child: Failed to open input file: " << _inputFile << "\n";
				exit(1);
			}
			dup2(inFd, STDIN_FILENO);
			close(inFd);
		}
		else
		{
			close(STDIN_FILENO);
			int nullFd = open("/dev/null", O_RDONLY);
			dup2(nullFd, STDIN_FILENO);
			close(nullFd);
		}
		
		int outFd = open(_outputFile.c_str(), O_WRONLY | O_TRUNC);
		if (outFd < 0)
		{
			std::cerr << "Child: failed to open output file\n";
			exit(1);
		}
		
		dup2(outFd, STDOUT_FILENO);
		close(outFd);
		std::map<std::string, std::string> envMap = setupEnvironment();
		char **envp = mapToEnvArray(envMap);
		std::string interpreter = getCGIInterpreter(_scriptPath);
		std::cerr << "interpreter = " << interpreter << "\n";
		std::cerr << "script Path = " << _scriptPath << "\n";
		// std::cerr << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
		if (!interpreter.empty())
		{
			char *argv[3];
			argv[0] = const_cast<char*>(interpreter.c_str());
			argv[1] = const_cast<char*>(_scriptPath.c_str());
			argv[2] = NULL;
			execve(interpreter.c_str(), argv, envp);
		}
		else
		{
			char *argv[2];
			argv[0] = const_cast<char*>(_scriptPath.c_str());
			argv[1] = NULL;
			execve(_scriptPath.c_str(), argv, envp);
		}
		freeEnvArray(envp);
		std::cerr << "execve failed: " << strerror(errno) << "\n";
		exit(1);
	}
	else
	{
		_outFile = open(_outputFile.c_str(), O_RDONLY | O_NONBLOCK);
		if (_outFile < 0)
		{
			std::cerr << "Parent: Failed to open output File\n";
			waitpid(_pid, NULL, 0);
			unlink(_outputFile.c_str());
			return false;
		}
		_startTime = time(NULL);

		// std::cout << "CGI started (PID " << _pid << ")" << std::endl;
		// std::cout << "  Input:  " << (_inputFile.empty() ? "(none)" : _inputFile) << std::endl;
		// std::cout << "  Output: " << _outputFile << std::endl;

		return true;
	}
}

void CGI::appendOutput(const char* data, size_t size)
{
	_output.append(data, size);
}

std::string CGI::formatResponse()
{
	// Check if CGI output already has HTTP status
	if (_output.find("HTTP/") == 0)
		return _output;

	// Check if output has headers
	size_t headerEnd = _output.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		headerEnd = _output.find("\n\n");

	if (headerEnd != std::string::npos)
	{
		// CGI provided headers
		return "HTTP/1.0 200 OK\r\n" + _output;
	}

	// No headers - add default
	return "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" + _output;
}