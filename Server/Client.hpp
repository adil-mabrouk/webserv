#pragma once

#include "Server.hpp"
#include "../Configuration/config.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include <cstdlib>

class CGI;

class Client {
	public:
		enum State	{ READING, READ_HEADER, READ_BODY, CGI_RUNNING, CGI_WRITING, CGI_HEADERS_WRITING, WRITING, ERROR_HEADERS_WRITING, ERROR_WRITING, DONE };

		Client(int fd, ServerConfig config);
		int			getState() const;
		void		setState(State state);
		void		setFileFd(int);
		int			getFileFd();
		void		setFileName(string);
		void		setErrorStatus(int);
		bool		readRequest();  
		void		processRequest();  
		bool		writeResponse();  
		bool		writeCGIResponse();
		bool		writeErrorResponse();
		int			writeErrorResponseHeaders();
		void		postInit();
		~Client();
		const ServerConfig	&getServerConfig() const;

		std::string		_resRes;
		
		CGI				*getCGI() const;

	private:
		Request			requestHandle;
		ServerConfig	_serverConfig;
		LocationConfig	location_config;
		int				_fd;
		State			_state;
		std::string		_resBuff;
		size_t			_byteSent;
		std::ofstream*	upload_file;
		unsigned long		content_length;
		LocationConfig	findLocation();
		std::string		fileName;
		int				fileFd;
		int				errorStatus;
		unsigned long	contentSize;

		 

		std::string		_inputFileName;
		CGI	*_cgi;
		
		bool			isCGIRequest(const std::string &path);
		std::string		startCGI();
		 

	 

};
