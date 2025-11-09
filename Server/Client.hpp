#pragma once

#include "Server.hpp"

class Client {
	public:
		std::string	_resBuff;
		size_t		_byteSent;

		enum State	{ READING, WRITING, PROCESSING, DONE };

		Client(int fd);
		int		getState() const;
		void	setState(State state);
		bool	readRequest(); // Returns true if request is complete
		void	processRequest(); // Process the request and prepare the response
		bool	writeResponse(); // Returns true if response is fully sent
		void	resetForNextRequest(); // Reset client state for next request

	private:
		int		_fd;
		State	_state;
};