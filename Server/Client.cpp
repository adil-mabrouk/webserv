#include "Client.hpp"

Client::Client(int fd)
{
	_fd = fd;
	_resBuff = "";
	_byteSent = 0;
	_state = READING;
}

int	Client::getState() const
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
		std::cerr << "read() error: " << strerror(errno) << "\n";
		setState(DONE);
		return false;
	}
	else if (bytesRead == 0)
	{
		std::cout << "Client disconnected fd = " << _fd << "\n";
		setState(DONE);
		return false;
	}
	else
	{
		// std::cout << "Read " << bytesRead << " bytes from client fd = " << _fd << "\n";
		setState(PROCESSING);
		return true;
	}
}

bool	Client::writeResponse()
{
	size_t		remain = _resBuff.size() - _byteSent;
	const char*	data = _resBuff.c_str() + _byteSent;
	ssize_t		bytesWriten = write(_fd, data, remain);
	if (bytesWriten < 0)
	{
		std::cerr << "write() error: " << strerror(errno) << "\n";
		setState(DONE);
		return false;
	}
	_byteSent += bytesWriten;
	if (_byteSent >= _resBuff.size())
	{
		setState(DONE);
		return true;
	}
	return false;
}

void	Client::resetForNextRequest()
{
	_resBuff.clear();
	_byteSent = 0;
	_state = READING;
}

void	Client::processRequest()
{
	_resBuff = "HTTP/1.1 200 OK\r\n"
			   "Content-Length: 13\r\n"
			   "Connection: close\r\n"
			   "\r\n"
			   "Hello, World!";
	setState(WRITING);
}
