#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0),
	  _lastActivity(time(NULL))
{
}

void	Client::updateActivity() {
	_lastActivity = time(NULL);
}

bool	Client::isTimedOut(int timeoutSeconds) const {
	time_t	now = time(NULL);
	time_t	elapsed = now - _lastActivity;
	return elapsed > timeoutSeconds;
}

time_t	Client::getLastActivity() const {
	return _lastActivity;
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
		updateActivity();
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
	updateActivity();
	if (_byteSent >= _resBuff.size())
	{
		setState(DONE);
		return true;
	}
	return false;
}

// void	Client::resetForNextRequest() // for keep alive
// {
// 	_resBuff.clear();
// 	_byteSent = 0;
// 	_state = READING;
// 	updateActivity();
// }

void	Client::processRequest()
{
	_resBuff = "HTTP/2 200 OK\r\n"
			   "Content-Length: 13\r\n"
			   "Connection: close\r\n"
			   "\r\n"
			   "Hello, World!";
	setState(WRITING);
}
