#include "Client.hpp"

Client::Client(int fd, ServerConfig *config)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0),
	  _headerEndPos(0),
	  _contentLength(0) // Remove Later (Hardcoded)
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

	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
	if (bytesRead <= 0)
	{
		if (bytesRead < 0)
			std::cout << "recv() error: " << strerror(errno) << "\n";
		setState(DONE);
		return false;
	}
	_resBuff.append(buffer, bytesRead);
	if (getState() == READING)
	{
		_headerEndPos = _resBuff.find("\r\n\r\n");
		if (_headerEndPos != std::string::npos)
		{
			requestHandle.request_parsing(_resBuff);
			if (requestHandle.request_line.getMethod() == "POST")
			{
				requestHandle.request_header.parse(string(_resBuff.begin(), _resBuff.begin() + _headerEndPos));
				for (std::map<const std::string, const std::string>::
					const_iterator	it = requestHandle.request_header.header_data.begin(); 
					it != requestHandle.request_header.header_data.end(); it++)
					{
						if (it->first == "Content-Length")
							_contentLength = std::strtol(it->second.c_str(), NULL, 10);
					}
				// setState(READ_HEADER);
				size_t bodyStart = _headerEndPos + 4;
				size_t currentBodySize = _resBuff.size() - bodyStart;
				if (currentBodySize >= _contentLength)
				{
					_resBuff = _resBuff.substr(0, bodyStart + _contentLength);
					return true;
				}
			}
			else // No body Expected (GET || DELETE)
				return true;
		}
	}
	return false;
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
