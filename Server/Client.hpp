#pragma once

#include "Server.hpp"

class Client {
	public:
		std::string	_resBuff;
		size_t		_byteSent;

		enum State	{ READING, WRITING, PROCESSING, DONE };

		Client(int fd);
		int		getState();
		void	setState(State state);
		bool	readRequest();
		void	processRequest();
		bool	writeResponse();
		bool	shouldkeepAlive();
		void	resetForNextRequest();

	private:
		int		_fd;
		State	_state;
};