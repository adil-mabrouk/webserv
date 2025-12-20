#pragma once

#include "Server.hpp"
#include "../Configuration/config.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include <cstdlib>

class Client {
	public:

		enum State	{ READING, READ_REQUET_LINE, READ_HEADER, READ_BODY, WRITING, PROCESSING, DONE };

		Client(int fd, ServerConfig config);
		int			getState() const;
		void		setState(State state);
		bool		readRequest(); // Returns true if request is complete
		void		processRequest(); // Process the request and prepare the response
		bool		writeResponse(); // Returns true if response is fully sent
		void		postInit();
		~Client();
		const ServerConfig	&getServerConfig() const;

		std::string		_resRes;
	private:
		Request			requestHandle;
		ServerConfig	_serverConfig;
		int				_fd;
		State			_state;
		std::string		_resBuff;
		size_t			_byteSent;
		std::ofstream*	upload_file;
		long long		content_length;
};
