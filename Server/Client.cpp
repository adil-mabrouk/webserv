#include "Client.hpp"

Client::Client(int fd, ServerConfig *config)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0)
{
	(void)config;
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
			// std::cout << "Client disconnected\n";
			return false;
		}
		_resBuff.append(buffer, bytesRead);
		if (_resBuff.find("\r\n\r\n") != std::string::npos)
		{
			setState(READ_HEADER);
			// cout << _resBuff << '\n';
			// for (string::iterator it = _resBuff.begin(); it != _resBuff.end(); it++)
			// {
			// 	if (*it == '\n')
			// 		cout << "\\n";
			// 	else if (*it == '\r')
			// 		cout << "\\r";
			// 	else
			// 		cout << *it;
			// }
			return true;
		}
	}
}

void	Client::processRequest()
{
	requestHandle.request_parsing(_resBuff);

	std::string httpResponse = requestHandle.request_exec();
	_resBuff = httpResponse;
	_byteSent = 0;
	setState(WRITING);
}

bool	Client::writeResponse()
{
	while (_byteSent < _resBuff.size())
	{
		ssize_t bytesSent = send(_fd, _resBuff.c_str() + _byteSent, _resBuff.size() - _byteSent, 0);
		if (bytesSent < 0)
		{
			std::cout << "send() error: " << strerror(errno) << "\n";
			return false;
		}
		_byteSent += bytesSent;
	}
	setState(DONE);
	return true;
}
