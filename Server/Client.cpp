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
	char				buffer[4096];
	bool				request_line_flag;
	bool				request_headers_flag;
	size_t				crlf_index;

	request_line_flag = false;
	request_headers_flag = false;
	while (true)
	{
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
		if (!request_line_flag)
		{
			crlf_index = _resBuff.find("\r\n");
			if (crlf_index != string::npos)
			{
				requestHandle.request_line.parse(string(_resBuff.begin(),
											_resBuff.begin() + crlf_index));
				_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
				request_line_flag = true;
			}
		}
		if (!request_headers_flag)
		{
			crlf_index = _resBuff.find("\r\n\r\n");
			if (crlf_index != string::npos)
			{
				requestHandle.request_header.parse(string(_resBuff.begin(),
											_resBuff.begin() + crlf_index + 2));
				if (requestHandle.request_line.getMethod() == "POST")
				{
					Response	response(requestHandle);

					if (requestHandle.request_header.getHeaderData().find("Content-Length") ==
						requestHandle.request_header.getHeaderData().end() ||
						requestHandle.request_header.getHeaderData().find("Content-Type") ==
						requestHandle.request_header.getHeaderData().end())
						cerr << "1\n", exit (1);
					response.POSTResource(_fd, string(_resBuff.begin() + crlf_index + 4, 
									  _resBuff.end()));
					_resBuff = response.getResponse();
				}
				request_headers_flag = true;
			}
			return (true);
		}
	}
}

void	Client::processRequest()
{
	if (requestHandle.request_line.getMethod() != "POST")
	{
		requestHandle.requestParsing(_resBuff);

		std::string httpResponse = requestHandle.requestExec();
		_resBuff = httpResponse;
	}
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
