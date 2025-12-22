#include "Client.hpp"
#include "../CGI/cgi.hpp"

Client::Client(int fd, ServerConfig config)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0),
	  upload_file(NULL),
	  content_length(0),
	  _cgi(NULL)
{
	_serverConfig = config;
}

Client::~Client()
{
	delete upload_file;
	delete _cgi;
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
	if (it_content_type == requestHandle.request_header.getHeaderData().end())
		cerr << "type not found\n", exit(1);
	if (it_content_length == requestHandle.request_header.getHeaderData().end())
		cerr << "length not found\n", exit(1);
	content_length = std::strtol(it_content_length->second.c_str(), NULL, 0);
	// cout << "address: " << _serverConfig. << '\n';
	// cout << "_--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	if (content_length > _serverConfig.max_body_size)
		throw 400;
	std::srand(std::time(NULL));
	oss << std::rand();
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
			requestHandle.request_line.parse(string(_resBuff.begin(),
										   _resBuff.begin() + crlf_index));
			_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
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
			requestHandle.request_header.parse(string(_resBuff.begin(),
											 _resBuff.begin() + crlf_index + 2));
			_resBuff.assign(_resBuff.begin() + crlf_index + 4, 
				   _resBuff.end());
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

void	Client::processRequest()
{
	// cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
	// cout << "=> request_line:\n\t|" << requestHandle.request_line.getMethod()
	// 	<< "| |" << requestHandle.request_line.getURI() << "|\n";
	// cout << "=> request headers:\n";
	// for (map<const string, const string>::const_iterator it = requestHandle.request_header.getHeaderData().begin();
	// 	it != requestHandle.request_header.getHeaderData().end(); it++)
	// 	cout << "\t|" << it->first << "| |" << it->second << "|\n";
	// cout << "=> request body:\n|" << requestHandle.body << "|\n";
	std::string path = requestHandle.request_line.getURI();
	if (true)
	{
		startCGI();
		return ;
	}
	if (requestHandle.request_line.getMethod() != "POST")
	{
		// requestHandle.requestParsing(_resBuff);

		std::string httpResponse = requestHandle.requestExec();
		_resRes += httpResponse;
	}
	else if (requestHandle.request_line.getMethod() == "POST")
	{
		cout << "here\n";
		_resRes = "HTTP/1.0 201 Created\r\n\r\n";
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

std::string	Client::mapURLToFilePath(const std::string &urlPath)
{
	std::string root = _serverConfig.root;
	if (root.empty())
		cerr << "Error: ROOT empty\n";
	std::string	path = urlPath;
	size_t	queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);
	return root + path;
}

bool	Client::isCGIRequest(const std::string &path)
{
	if (path.find("/cgi-bin/") == 0)
		return true;
	if (path.find(".php") != std::string::npos 
		|| path.find(".py") != std::string::npos)
		return true;
	return false;
}

void	Client::startCGI()
{
	// std::string urlPath = requestHandle.request_line.getURI();
	std::string scriptPath;
	// if (x == 1)
	// {
	// 	x = 0;
		// scriptPath = "www/var/timeout.php";
	// }
	// else
		scriptPath = "www/var/test.php";
	if (access(scriptPath.c_str(), F_OK) != 0)
	{
		_resRes = "HTTP/1.0 404 NOt Found\r\n\r\nCGI Script not found";
		setState(WRITING);
		return ;
	}
	_cgi = new CGI();
	_cgi->setScriptPath(scriptPath);
	_cgi->setMethod("GET");
	_cgi->setQueryString("name=ADIL");

	// if (requestHandle.request_line.getMethod() == "POST")
	// 	;// _cgi->setBody();
	// const std::map<const std::string, const std::string> &headers = 
	// 	requestHandle.request_header.getHeaderData();

	// for (std::map<const std::string, const std::string>::const_iterator it = headers.begin(); 
	// 	it != headers.end(); it++)
	// {
	// 	_cgi->setHeader(it->first, it->second);
	// }
	_cgi->setHeader("Host", "localhost:8080");
	_cgi->setHeader("Accept", "text/html,application/json");
	_cgi->setHeader("Accept-Language", "en-US,en;q=0.9");
	_cgi->setHeader("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");
	if (_cgi->start())
		setState(CGI_RUNNING);
	
	else
	{
		delete _cgi;
		_cgi = NULL;
		_resRes = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
		setState(WRITING);
	}
}
