#pragma once

#include "../Server/Client.hpp"

class CGI
{
	public:
		CGI();
		~CGI();

		void	setScriptPath(const std::string &path);
		void	setMethod(const std::string &method);
		void	setQueryString(const std::string &query);
		void	setInputFile(const std::string &filePath);
		void	setHeader(const std::string &key, const std::string &value);

		pid_t	getPid() const;
		int		getOutFile() const;
		time_t	getStartTime() const;
		const	std::string	&getOutput() const;

		std::string	start();
		void	appendOutput(const char* data, size_t size);
		std::string	formatResponse();

	private:
		std::string							_scriptPath;
		std::string							_method;
		std::string							_queryString;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		pid_t								_pid;
		int									_outFile;
		time_t								_startTime;
		std::string							_inputFile;
		std::string							_outputFile;
		std::string							_output;

	std::string	getCGIInterpreter(const std::string &path);
	std::map<std::string, std::string> setupEnvironment();
	char	**mapToEnvArray(const std::map<std::string, std::string>& env);
	void	freeEnvArray(char **envp);
};