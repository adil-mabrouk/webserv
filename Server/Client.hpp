#pragma once

#include "Server.hpp"

class Client {
	public:
		enum State	{
			READING,
			WRITING,
			PROCESSING,
			DONE
		};
		Client(int fd);
		int		getState();
		void	setState(State state);
		bool	readRequest();
		void	processRequest();
		bool	writeResponse();
	private:
		int		_fd;
		State	_state;

};