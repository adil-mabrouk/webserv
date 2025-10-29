#pragma once

#include "Server.hpp"

class Client {
	public:
		enum State {
			READING,
			WRITING,
			PROCESSING,
			DONE
		};
		Client(int fd);
		int	getState();
	private:
		int		_fd;
		State	_state;

};