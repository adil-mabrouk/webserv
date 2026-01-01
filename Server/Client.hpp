#pragma once

#include "Server.hpp"
#include "../Configuration/config.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include <cstdlib>

class CGI;

class Client {
	public:
		enum State	{ READING, READ_HEADER, READ_BODY, CGI_RUNNING, CGI_WRITING, CGI_HEADERS_WRITING, WRITING, PROCESSING, ERROR_HEADERS_WRITING, ERROR_WRITING, DONE };

		Client(int fd, ServerConfig config);
		int			getState() const;
		void		setState(State state);
		void		setFileFd(int);
		int			getFileFd();
		void		setFileName(string);
		void		setErrorStatus(int);
		bool		readRequest(); // Returns true if request is complete
		void		processRequest(); // Process the request and prepare the response
		bool		writeResponse(); // Returns true if response is fully sent
		bool		writeCGIResponse();
		bool		writeErrorResponse();
		int			writeErrorResponseHeaders();
		void		postInit();
		~Client();
		const ServerConfig	&getServerConfig() const;
		void		setServerInfo(const std::string &host, int port);

		std::string		_resRes;
		
		CGI				*getCGI() const { return _cgi; }

	private:
		Request			requestHandle;
		ServerConfig	_serverConfig;
		LocationConfig	location_config;
		std::string		_serverHost;
		int				_serverPort;
		int				_fd;
		State			_state;
		std::string		_resBuff;
		size_t			_byteSent;
		std::ofstream*	upload_file;
		long long		content_length;
		LocationConfig	findLocation();
		long long		_outputLength;
		std::string		fileName;
		int				fileFd;
		int				errorStatus;


		// ALL CGI needs {

		std::string		_inputFileName;
		CGI	*_cgi;
		
		bool			isCGIRequest(const std::string &path);
		std::string		startCGI();
		// std::string		mapURLToFilePath(const std::string &urlPath);

	// }

};
