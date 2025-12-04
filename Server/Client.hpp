#pragma once

#include "Server.hpp"
#include "../Configuration/config.hpp"
#include "../Request/Request.hpp"

class Client {
	public:

		enum State	{ READING, READ_HEADER, WRITING, PROCESSING, DONE };

		Client(int fd, ServerConfig *config);
		int			getState() const;
		void		setState(State state);
		bool		readRequest(); // Returns true if request is complete
		void		processRequest(); // Process the request and prepare the response
		bool		writeResponse(); // Returns true if response is fully sent
		// ServerConfig	*getServerConfig() const;

	private:
		Request			requestHandle;
		ServerConfig	*_serverConfig;
		int				_fd;
		State			_state;
		std::string		_resBuff;
		size_t			_byteSent;
		size_t			_headerEndPos;
		size_t			_contentLength;
};