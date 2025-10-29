#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_state = READING;
}

int	Client::getState()
{
	return _state;
}