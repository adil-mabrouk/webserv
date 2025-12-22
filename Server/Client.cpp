#include "Client.hpp"
#include <algorithm>

Client::Client(int fd, ServerConfig config)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0),
	  upload_file(NULL),
	  content_length(0)
{
	_serverConfig = config;
	location_config = NULL;
}

Client::~Client()
{
	delete upload_file;
}

int	Client::getState() const
{
	return _state;
}

void	Client::setState(State state)
{
	_state = state;
}

const ServerConfig	&Client::getServerConfig() const
{
	return _serverConfig;
}

void	Client::postInit()
{
	ostringstream									oss;
	map<const string, const string>::const_iterator	it_content_type;
	map<const string, const string>::const_iterator	it_content_length;

	it_content_type = requestHandle.request_header.getHeaderData().find("Content-Type");
	it_content_length = requestHandle.request_header.getHeaderData().find("Content-Length");
	if (it_content_length == requestHandle.request_header.getHeaderData().end())
		throw 400;
	content_length = std::strtol(it_content_length->second.c_str(), NULL, 0);
	if (content_length > _serverConfig.max_body_size)
		throw 400;
	std::srand(std::time(NULL));
	oss << std::rand();
	if (it_content_type == requestHandle.request_header.getHeaderData().end())
		upload_file	= new std::ofstream(("upload_" + oss.str()
								  + "." + Response::fillContentType("x-www-form-urlencoded", 1)).c_str());
	else
		upload_file	= new std::ofstream(("upload_" + oss.str()
								  + "." + Response::fillContentType(it_content_type->second, 1)).c_str());
}

bool	Client::readRequest()
{
	char				buffer[4113394];
	size_t				crlf_index;

	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
	if (bytesRead < 0)
	{
		std::cout << "recv() error: " << strerror(errno) << "\n";
		return false;
	}
	if (bytesRead == 0)
		return true; // was false

	_resBuff.append(buffer, bytesRead);
	if (getState() == READING)
	{
		crlf_index = _resBuff.find("\r\n");
		if (crlf_index != string::npos)
		{
			vector <string>::iterator	it;

			requestHandle.request_line.parse(string(_resBuff.begin(),
										   _resBuff.begin() + crlf_index));
			_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
			location_config = findLocation();
			if (!location_config)
				throw 404;
			it = find(location_config->methods.begin(), location_config->methods.end(), requestHandle.request_line.getMethod());
			if (it == location_config->methods.end())
				throw 403;
			setState(READ_HEADER);
		}
		else
			return (false);
	}
	if (getState() == READ_HEADER)
	{
		crlf_index = _resBuff.find("\r\n\r\n");
		if (crlf_index != string::npos)
		{
			map<const string, const string>::const_iterator	it;

			requestHandle.request_header.parse(string(_resBuff.begin(),
											 _resBuff.begin() + crlf_index + 2));
			_resBuff.assign(_resBuff.begin() + crlf_index + 4, 
				   _resBuff.end());
			it = requestHandle.request_header.getHeaderData().find("Content-Length");
			if (it != requestHandle.request_header.getHeaderData().end())
				if (std::strtol(it->second.c_str(), NULL, 0) > _serverConfig.max_body_size)
					throw 400;
			if (requestHandle.request_line.getMethod() == "POST")
				setState(READ_BODY), postInit();
		}
		else
			return (false);
	}
	if (getState() == READ_BODY && upload_file)
	{
		*upload_file << string(_resBuff.begin(), _resBuff.end());
		upload_file->flush(), _resBuff.clear();
		if (upload_file->tellp() < content_length)
			return (false);
	}
	return (true);
}

LocationConfig*	Client::findLocation()
{
	string	uri = requestHandle.request_line.getURI();

	while (uri.size())
	{
		for (map<string, LocationConfig>::iterator	it = _serverConfig.locations.begin();
			it != _serverConfig.locations.end(); it++)
			if (!it->first.compare(uri))
				return (&it->second);
		if (uri[uri.size() - 1] == '/')
			uri.erase(uri.size() - 1, 1);
		else
		{
			size_t	index;
			
			index = uri.rfind('/', uri.size());
			if (index == string::npos)
				return (NULL);
			uri.erase(uri.begin() + index + 1, uri.end());
		}
	}
	return (NULL);
}

void	Client::processRequest()
{
	if (requestHandle.request_line.getMethod() != "POST")
	{
		LocationConfig*							location_config;

// how about if the location isn't specified in the config
		location_config = findLocation();
		if (!location_config)
			throw 400;
		Response	response(requestHandle, *location_config);

		if (!requestHandle.request_line.getMethod().compare("DELETE"))
			response.DELETEResource();
		else if (!requestHandle.request_line.getMethod().compare("GET"))
			response.GETResource();
		_resRes += response.getResponse();
	}
	else if (requestHandle.request_line.getMethod() == "POST")
	{
		cout << "here\n";
		_resRes = "HTTP/1.1 201 Created\r\n\r\n";
	}
	_byteSent = 0;
 	setState(WRITING);
}

bool	Client::writeResponse()
{
	// cout << '\n' << _resRes << "\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n";
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
