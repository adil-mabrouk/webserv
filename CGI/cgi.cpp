#include "cgi.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

// extern char** environ;

CGI::CGI() 
	: _pid(-1), _pipeOut(-1), _startTime(0)
{
}

CGI::~CGI()
{
	if (_pipeOut != -1)
		close(_pipeOut);
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

void CGI::setBody(const std::string& body)
{
	_body = body;
}

void CGI::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

pid_t CGI::getPid() const
{
	return _pid;
}

int CGI::getPipeOut() const
{
	return _pipeOut;
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
	int pipeOut[2];
	int pipeIn[2];
	if (pipe(pipeOut) < 0 || pipe(pipeIn) < 0)
	{
		std::cerr << "Pipe creation failed\n";
		return false;
	}
	_pid = fork();
	if (_pid < 0)
	{
		close(pipeOut[0]);
		close(pipeOut[1]);
		close(pipeIn[0]);
		close(pipeIn[1]);
		std::cerr << "Fork failed\n";
		return false;
	}

	if (_pid == 0) // Child
	{
		// Setup stdin/stdout
		close(pipeIn[1]);
		close(pipeOut[0]);

		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);

		close(pipeIn[0]);
		close(pipeOut[1]);

		// Setup environment
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
		std::cerr << "execve failed\n";
		exit(1);
	}
	else // Parent
	{
		close(pipeOut[1]);
		close(pipeIn[0]);

		// Write body to CGI stdin
		if (!_body.empty())
		{
			write(pipeIn[1], _body.c_str(), _body.length());
		}
		close(pipeIn[1]);

		// Set pipe to non-blocking
		int flags = fcntl(pipeOut[0], F_GETFL, 0);
		fcntl(pipeOut[0], F_SETFL, flags | O_NONBLOCK);

		_pipeOut = pipeOut[0];
		_startTime = time(NULL);
		_output.clear();

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