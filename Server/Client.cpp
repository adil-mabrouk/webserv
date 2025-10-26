#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd), _requestComplete(false), _bytesSent(0), _readyToWrite(false) {}

Client::~Client() { close(_fd); }

// Getters
int	Client::getFd() const { return _fd; }

bool	Client::requestComplete() const { return _requestComplete; }

bool	Client::readyToWrite() const { return _readyToWrite; }

const std::string&	Client::getRequest() const { return _requestBuffer; }

const std::string&	Client::getResponse() const { return _responseBuffer; }

// Setters
void	Client::setResponse(const std::string &response)
{
	_responseBuffer = response;
	_bytesSent = 0;
}

void	Client::markReadyToWrite(bool state) { _readyToWrite = state; }

// I/O methods
bool	Client::readRequest()
{
	char buffer[4096];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
	if (bytesRead < 0)
		return false; // Error
	if (bytesRead == 0)
		return false; // Connection closed
	_requestBuffer.append(buffer, bytesRead);
	return true;
}

bool	Client::writeResponse()
{
	if (_bytesSent >= _responseBuffer.size())
		return true; // Nothing to send

	ssize_t bytesLeft = _responseBuffer.size() - _bytesSent;
	ssize_t bytesSentNow = send(_fd, _responseBuffer.c_str() + _bytesSent, bytesLeft, 0);
	if (bytesSentNow < 0)
		return false; // Error

	_bytesSent += bytesSentNow;
	return true;
}

bool	Client::isDone() const
{
	return _bytesSent >= _responseBuffer.size();
}

// Request handling
void	Client::reset()
{
	_requestBuffer.clear();
	_responseBuffer.clear();
	_requestComplete = false;
	_bytesSent = 0;
	_readyToWrite = false;
}
