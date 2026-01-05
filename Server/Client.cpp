#include "Client.hpp"
#include <algorithm>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include "../CGI/cgi.hpp"

Client::Client(int fd, ServerConfig config)
	: _fd(fd),
	  _state(READING),
	  _resBuff(""),
	  _byteSent(0),
	  upload_file(NULL),
	  content_length(0),
	  contentSize(0),
	  _cgi(NULL)
{
	_serverConfig = config;
	std::cout << "\n\nconstructor called ++++\n\n";
}

Client::~Client()
{
	std::cout << "\n\ndestructor called ----\n\n";
	close(_fd);
	_fd = -1;
	delete upload_file;
	upload_file = NULL;
	delete _cgi;
	_cgi = NULL;
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

LocationConfig	Client::findLocation()
{
	string	uri = requestHandle.request_line.getURI();

	while (uri.size())
	{
		for (map<string, LocationConfig>::iterator	it = _serverConfig.locations.begin();
			it != _serverConfig.locations.end(); it++)
			if (!it->first.compare(uri))
				return (it->second);
		if (uri[uri.size() - 1] == '/')
			uri.erase(uri.size() - 1, 1);
		else
		{
			size_t	index;
			
			index = uri.rfind('/', uri.size());
			if (index == string::npos)
				throw 404;
			uri.erase(uri.begin() + index + 1, uri.end());
		}
	}
	throw 404;
}

void	Client::postInit()
{
	ostringstream									oss;
	map<const string, const string>::const_iterator	it_content_type;
	map<const string, const string>::const_iterator	it_content_length;

	it_content_type = requestHandle.request_header.getHeaderData().find("Content-Type");
	it_content_length = requestHandle.request_header.getHeaderData().find("Content-Length");
	if (it_content_length == requestHandle.request_header.getHeaderData().end())
		cout << "content-length not found\n", throw 400;
	content_length = std::strtol(it_content_length->second.c_str(), NULL, 0);
	if (content_length > _serverConfig.max_body_size)
		cout << "body size too large 2\n", throw 400;
	if (!location_config.allow_upload)
		cout << "upload not allowed\n", throw 403;
	std::srand(std::time(NULL));
	oss << std::rand();
	if (it_content_type == requestHandle.request_header.getHeaderData().end())
	{
		if (isCGIRequest(requestHandle.request_line.getURI()))
		{
			upload_file	= new std::ofstream(("/tmp/upload_" + oss.str() + "."
								   + Response::fillContentType("x-www-form-urlencoded", 1)).c_str());
			_inputFileName = "/tmp/upload_" + oss.str() + "."
				+ Response::fillContentType("x-www-form-urlencoded", 1);
		}
		else
		{
			upload_file	= new std::ofstream((_serverConfig.root + location_config.upload_store
								   + "/upload_" + oss.str() + "."
								   + Response::fillContentType("x-www-form-urlencoded", 1)).c_str());
			_inputFileName = _serverConfig.root + location_config.upload_store + "/upload_"
				+ oss.str() + "." + Response::fillContentType("x-www-form-urlencoded", 1);
		}
	}
	else
	{
		if (isCGIRequest(requestHandle.request_line.getURI()))
		{
			upload_file	= new std::ofstream(("/tmp/upload_" + oss.str() + "."
								   + Response::fillContentType(it_content_type->second, 1)).c_str());
			_inputFileName = "/tmp/upload_" + oss.str() + "."
				+ Response::fillContentType(it_content_type->second, 1);
		}
		else
		{
			upload_file	= new std::ofstream((_serverConfig.root + location_config.upload_store
								   + "/upload_" + oss.str() + "."
								   + Response::fillContentType(it_content_type->second, 1)).c_str());
			_inputFileName = _serverConfig.root + location_config.upload_store
				+ "/upload_" + oss.str() + "." + Response::fillContentType(it_content_type->second, 1);
		}
		
	}
	if (!upload_file->is_open())
		throw 404;
}

bool	Client::readRequest()
{
	char	buffer[200000];
	size_t	crlf_index;

	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
	if (bytesRead <= 0)
	{
		setState(DONE);
		return true;
	}
	_resBuff.append(buffer, bytesRead);
	if (getState() == READING)
	{
		crlf_index = _resBuff.find("\r\n");
		if (crlf_index != string::npos)
		{
			vector <string>::iterator	it;

			cout << "- - - request line parsing\n";
			requestHandle.request_line.parse(string(_resBuff.begin(),
										   _resBuff.begin() + crlf_index));
			 
			_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
			location_config = findLocation();
			cout << "- - - location: " << location_config.path << '\n';
			it = find(location_config.methods.begin(), location_config.methods.end(),
			 requestHandle.request_line.getMethod());
			if (!location_config.redirectExist && it == location_config.methods.end())
				cout << "method not allowed\n", throw 403;
			requestHandle.request_line.removeDupSl();
			requestHandle.request_line.removeDotSegments();
			requestHandle.request_line.rootingPath(location_config.path, location_config.root, _serverConfig.root);
			cout << "- - - uri: " << requestHandle.request_line.getURI() << '\n';
			cout << "- - - request line parsing done\n";
			setState(READ_HEADER);
		}
		else
			throw 400;
	}
	if (getState() == READ_HEADER)
	{
		crlf_index = _resBuff.find("\r\n\r\n");
		if (crlf_index != string::npos)
		{
			map<const string, const string>::const_iterator	it;

			cout << "- - - request headers parsing\n";
			requestHandle.request_header.parse(string(_resBuff.begin(),
											 _resBuff.begin() + crlf_index + 2));
			_resBuff.assign(_resBuff.begin() + crlf_index + 4, 
				   _resBuff.end());
			it = requestHandle.request_header.getHeaderData().find("Content-Length");
			if (it != requestHandle.request_header.getHeaderData().end())
				if (std::strtoul(it->second.c_str(), NULL, 0) > _serverConfig.max_body_size)
					cout << "body size too large\n", throw 400;
			if (location_config.redirectExist)
				return (true);
			if (!location_config.cgi.size() && requestHandle.request_line.getMethod() == "POST")
				cout << "+ + + running POST\n", setState(READ_BODY), postInit();
			else if (location_config.cgi.size())
				if (requestHandle.request_line.getMethod() == "POST")
					cout << "+ + + running POST\n", setState(READ_BODY), postInit();
			cout << "- - - request headers parsing done\n";
		}
		else
			throw 400;
	}
	if (getState() == READ_BODY && upload_file)
	{
		unsigned long	size_tmp;

		cout << "- - - body reading\n";
		if (string(_resBuff.begin(), _resBuff.end()).size() + contentSize >= content_length)
		{
			size_tmp = (string(_resBuff.begin(), _resBuff.end()).size() + contentSize) - content_length; 
			*upload_file << string(_resBuff.begin(), _resBuff.end() - size_tmp);
			upload_file->flush(), _resBuff.clear();
		cout << "- - - body reading done\n";
		}
		else
		{
			*upload_file << string(_resBuff.begin(), _resBuff.end());
			contentSize += string(_resBuff.begin(), _resBuff.end()).size();
			upload_file->flush(), _resBuff.clear();
			return (false);
		}
	}
	return (true);
}

void	Client::processRequest()
{
	if (isCGIRequest(requestHandle.request_line.getURI()))
	{
		cout << "+ + + running cgi\n";
		fileName = startCGI();
		fileFd = open(fileName.c_str(), O_RDONLY);  
		if (-1 == fileFd)
			throw 500;
		setState(CGI_HEADERS_WRITING);
		_byteSent = 0;
		return ;
	}
	else if (location_config.redirectExist)
	{
		cout << "+ + + running http redirection\n";
		Response			response;

		cout << "http redirection: " << location_config.redirect.url << '\n';
		if (location_config.redirect.status_code == 301)
			response.statusCode301(location_config.redirect.url);
		else if (location_config.redirect.status_code == 302)
			response.statusCode302(location_config.redirect.url);
		else
			throw 501;
		_resRes = response.getResponse();
	}
	else if (requestHandle.request_line.getMethod() != "POST")
	{
		Response	response(requestHandle, location_config);

		if (!requestHandle.request_line.getMethod().compare("DELETE"))
			cout << "+ + + running DELETE\n",
			response.DELETEResource();
		else if (!requestHandle.request_line.getMethod().compare("GET"))
			cout << "+ + + running GET\n",
			response.GETResource();
		_resRes += response.getResponse();
	}
	else if (requestHandle.request_line.getMethod() == "POST")
	{
		if (requestHandle.request_header.getHeaderData()
			.find("Content-Length")->second == "0")
			_resRes = "HTTP/1.0 204 No Content\r\n\r\n";
		else
			_resRes = "HTTP/1.0 201 Created\r\n\r\n";
	}
	_byteSent = 0;
 	setState(WRITING);
}

bool	Client::writeResponse()
{
	std::cout << "Sending Response" << std::endl;
	// int fd2 = open("test.txt", O_CREAT | O_WRONLY);
	{
		int fd = open("Response.txt", O_CREAT | O_WRONLY | O_APPEND);
		write(fd, _resRes.c_str(), _resRes.size());
		close(fd);
		fd = -1;
	}
	// write (fd2, _resRes)
	send(_fd, _resRes.c_str(), _resRes.size(), 0);
	return (setState(DONE), 1);
}

bool	Client::writeCGIResponse()
{
	if (_cgi->getState() == CGI::CGI_RUNNING)
	{
		int status;
		pid_t result = waitpid(_cgi->getPid(), &status, WNOHANG);
	
		if (result == -1)
		{
			std::cerr << "waitpid error: " << strerror(errno) << std::endl;
			kill(_cgi->getPid(), SIGKILL);
			waitpid(_cgi->getPid(), NULL, WNOHANG);
			return false;
		}
		else if (result == 0)
			return false;
		else
		{
			if (WIFEXITED(status))
			{
				int exitCode = WEXITSTATUS(status);
				if (exitCode != 0)
				{
					std::cerr << "  CGI exited with error code " << exitCode << std::endl;
					throw 500;
				}
			}
			_cgi->setState(CGI::CGI_DONE);
			 
		}
	}
	ssize_t	bytesSent;
	char	buffer[4096];

	if (getState() == CGI_HEADERS_WRITING)
	{
		size_t			crlf_index;

		bytesSent = read(fileFd, buffer, sizeof(buffer) - 1);
		string	cgi_body(buffer);
		crlf_index = cgi_body.find("\r\n\r\n", 0);
		if (send(_fd, "HTTP/1.0 200 OK\r\n", 17, 0) <= 0)
		{
			setState(DONE);
			return true;
		}
		if (crlf_index != string::npos)
		{
			try
			{
				RequestHeader	response_header;

			response_header.parse(string(cgi_body.begin(), cgi_body.begin() + crlf_index + 2));
			}
			catch (int& status)
			{
				cout << "+ + + cgi headers error\n";
				throw 500;
			}
		}
		else
		{
			if (send(_fd, "\r\n", 2, 0) <= 0)
			{
				setState(DONE);
				return true;
			}
		}
		if (send(_fd, buffer, bytesSent, 0) <= 0)
		{
			setState(DONE);
			return true;
		}
		setState(CGI_WRITING);
	}
	bytesSent = read(fileFd, buffer, sizeof(buffer) -1);
	if (bytesSent == -1)
		throw 500;
	buffer[bytesSent] = '\0';
	 
	 
	 
	if (!bytesSent)
		return (setState(DONE), close(fileFd), fileFd = -1, true);
	bytesSent = send(_fd, buffer, bytesSent, 0);
	if (bytesSent <= 0)
	{
		setState(DONE);
		return true;
	}
	return false;
}

int	Client::writeErrorResponseHeaders()
{
	struct stat	st;

	if (stat(fileName.c_str(), &st) == -1)
	{
		Response	response;

		if (errorStatus == 400)
			response.statusCode400();
		else if (errorStatus == 401)
			response.statusCode401();
		else if (errorStatus == 403)
			response.statusCode403();
		else if (errorStatus == 404)
			response.statusCode404();
		else if (errorStatus == 500)
			response.statusCode500();
		else
			response.statusCode501();
		send(_fd, response.getResponse().c_str(),
		response.getResponse().size(), 0);
		return (setState(DONE), 1);
	}
	if (S_ISREG(st.st_mode))
	{
		std::stringstream	ss;
		string				response;
		size_t				content_length;

		content_length = st.st_size;
		ss << content_length;
		if (errorStatus == 400)
			response = "HTTP/1.0 400 Bad Request\r\n";
		else if (errorStatus == 401)
			response = "HTTP/1.0 401 Unauthorized\r\n";
		else if (errorStatus == 403)
			response = "HTTP/1.0 403 Forbidden\r\n";
		else if (errorStatus == 404)
			response = "HTTP/1.0 404 Not Found\r\n";
		else if (errorStatus == 500)
			response = "HTTP/1.0 500 Internal Server Error\r\n";
		else
			response = "HTTP/1.0 501 Not Implemented\r\n";
		response += "Content-Length: " + ss.str() + "\r\n\r\n";
 
		if (send(_fd, response.c_str(), response.size(), 0) <= 0)
		{
			setState(DONE);
			return true;
		}
		return (setState(ERROR_WRITING), 0);
	}
	else if (S_ISDIR(st.st_mode))
	{
		Response	res;

		res.statusCode403();

		{
			int fd = open("Response.txt", O_WRONLY | O_APPEND);
			write(fd, res.getResponse().c_str(), res.getResponse().size());
			close(fd);
			fd = -1;
		}

		send(_fd, res.getResponse().c_str(), res.getResponse().size(), 0);
		return (setState(DONE), 1);
	}
	else
		throw std::runtime_error("error page type not supported");
}

bool	Client::writeErrorResponse()
{
	char	buffer[4096];
	ssize_t	bytesSent;

	if (getState() == ERROR_HEADERS_WRITING)
		if (writeErrorResponseHeaders())
			return (true);
	bytesSent = read(fileFd, buffer, 4095);
	if (bytesSent == -1)
		(void)buffer;
	if (!bytesSent)
	{
		setState(DONE);
		close(fileFd);
		fileFd = -1;
		return (true);
	}

	{
		int fd = open("Response.txt", O_WRONLY | O_APPEND);
		write(fd, buffer, bytesSent);
		close(fd);
		fd = -1;
	}

	bytesSent = send(_fd, buffer, bytesSent, 0);
	if (bytesSent <= 0)
	{
		setState(DONE);
		return true;
	}
	return (false);
}

bool	Client::isCGIRequest(const std::string &path)
{
	size_t	extension_index;

	extension_index = path.rfind('.', path.size());
	if (extension_index == string::npos)
		return (false);
	for (vector<CGIConfig>::iterator it = location_config.cgi.begin(); it != location_config.cgi.end(); it++)
		if (string(path.begin() + extension_index, path.end()) == it->extension)
			return (true);
	return (false);
}

std::string	Client::startCGI()
{
	std::string scriptPath = requestHandle.request_line.getURI();

	if (access(scriptPath.c_str(), F_OK) != 0)
		throw 404;
	_cgi = new CGI();
	if (!_cgi)
		throw std::runtime_error("Failed allocation");
	_cgi->cgi_c = location_config.cgi;
	_cgi->setScriptPath(scriptPath);
	_cgi->setMethod(requestHandle.request_line.getMethod());
	_cgi->setQueryString(requestHandle.request_line.getQuery());

	if (requestHandle.request_line.getMethod() == "POST")
		_cgi->setInputFile(_inputFileName);
	const std::map<const std::string, const std::string> &headers = 
		requestHandle.request_header.getHeaderData();
	for (std::map<const std::string, const std::string>::const_iterator it = headers.begin();
		it != headers.end(); it++)
		_cgi->setHeader(it->first, it->second);
	try
	{
		std::string outFileName = _cgi->start();
		setState(CGI_RUNNING);
		return outFileName;
	}
	catch(int& status)
	{
		throw 500;
	}
}

void	Client::setFileFd(int fd)
{
	fileFd = fd;
}

int	Client::getFileFd()
{
	return (fileFd);
}

void	Client::setFileName(string name)
{
	fileName = name;
}

void	Client::setErrorStatus(int error_status)
{
	errorStatus = error_status;
}
