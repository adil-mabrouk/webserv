#include "Client.hpp"
#include <sstream>

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
	char				buffer[4113394];
	size_t				crlf_index;

	// while (true)
	// {
		ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
		if (bytesRead < 0)
		{
			std::cout << "recv() error: " << strerror(errno) << "\n";
			return false;
		}
		if (bytesRead == 0)
			return false;

		// cout << buffer << '\n';
		_resBuff.append(buffer, bytesRead);
		if (getState() == READING)
		{
			crlf_index = _resBuff.find("\r\n");
			if (crlf_index != string::npos)
			{
				requestHandle.request_line.parse(string(_resBuff.begin(),
											_resBuff.begin() + crlf_index));
				_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
				setState(READ_HEADER);
			}
		}
		if (getState() == READ_HEADER)
		{
			crlf_index = _resBuff.find("\r\n\r\n");
			if (crlf_index != string::npos)
			{
				requestHandle.request_header.parse(string(_resBuff.begin(),
											_resBuff.begin() + crlf_index + 2));
				requestHandle.body = string(_resBuff.begin() + crlf_index + 4, 
									 _resBuff.end());
				setState(READ_BODY);
			}
			else
				requestHandle.request_header.parse(string(_resBuff.begin(),
											_resBuff.end()));
		}
	return (true);
	// }
}

void	Client::processRequest()
{
	if (requestHandle.request_line.getMethod() != "POST")
	{
		requestHandle.requestParsing(_resBuff);

		std::string httpResponse = requestHandle.requestExec();
		_resRes += httpResponse;
	}
	else
	{
		if (requestHandle.request_line.getMethod() == "POST" && getState() == READ_BODY)
		{
			Response	response(requestHandle);

			if (requestHandle.request_header.getHeaderData().find("Content-Length") ==
				requestHandle.request_header.getHeaderData().end() ||
				requestHandle.request_header.getHeaderData().find("Content-Type") ==
				requestHandle.request_header.getHeaderData().end())
				cerr << "1\n", exit (1);
			response.POSTResource(_fd, requestHandle.body);
			_resRes = response.getResponse();
		}
	}
	_byteSent = 0;
// 	setState(WRITING);
}

bool	Client::writeResponse()
{
	while (_byteSent < _resRes.size())
	{
		ssize_t bytesSent = send(_fd, _resRes.c_str() + _byteSent, _resRes.size() - _byteSent, 0);
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
