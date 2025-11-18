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
	char buffer[4096];

	while (true)
	{
		ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
		if (bytesRead < 0)
		{
			std::cout << "recv() error: " << strerror(errno) << "\n";
			return false;
		}
		if (bytesRead == 0)
		{
			std::cout << "Client disconnected\n";
			return false;
		}
		_resBuff.append(buffer, bytesRead);
		if (_resBuff.find("\r\n\r\n") != std::string::npos)
		{
			setState(READ_HEADER);
			updateActivity();
			return true;
		}
	}
}

void	Client::processRequest()
{
	std::string httpResponse =
		"HTTP/1.1 200 OK\r\n"
		"Content-Length: 13\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Hello, World!";
	_resBuff = httpResponse;
	_byteSent = 0;
	setState(WRITING);
	updateActivity();
}

// void	Client::resetForNextRequest() // for keep alive
// {
// 	_resBuff.clear();
// 	_byteSent = 0;
// 	_state = READING;
// 	updateActivity();
// }
