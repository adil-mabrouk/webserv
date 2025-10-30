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

void	Client::setState(State state)
{
	_state = state;
}

bool	Client::readRequest()
{
	char	buffer[1024];
	ssize_t	bytesRead = read(_fd, buffer, sizeof(buffer));

	if (bytesRead < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		std::cerr << "read() error: " << strerror(errno) << "\n";
		setState(DONE);
		return false;
	}
	else if (bytesRead == 0)
	{
		std::cout << "Client disconnected fd = " << _fd << "\n";
		_state = DONE;
		return false;
	}
	else
	{
		std::cout << "Read " << bytesRead << " bytes from client fd = " << _fd << "\n";
		setState(PROCESSING);
		return true;
	}
}