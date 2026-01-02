#include "Client.hpp"
#include <algorithm>
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
		throw 400;
	content_length = std::strtol(it_content_length->second.c_str(), NULL, 0);
	if (content_length > _serverConfig.max_body_size)
		throw 400;
	std::srand(std::time(NULL));
	oss << std::rand();
	if (!location_config.allow_upload)
		throw 403;
	if (it_content_type == requestHandle.request_header.getHeaderData().end())
	{
		upload_file	= new std::ofstream((_serverConfig.root + location_config.upload_store
								  + "/upload_" + oss.str() + "."
								  + Response::fillContentType("x-www-form-urlencoded", 1)).c_str());
		_inputFileName = _serverConfig.root + location_config.upload_store + "/upload_"
			+ oss.str() + "." + Response::fillContentType("x-www-form-urlencoded", 1);
	}
	else
	{
		upload_file	= new std::ofstream((_serverConfig.root + location_config.upload_store
								  + "/upload_" + oss.str() + "."
								  + Response::fillContentType(it_content_type->second, 1)).c_str());
		_inputFileName = _serverConfig.root + location_config.upload_store
			+ "/upload_" + oss.str() + "." + Response::fillContentType(it_content_type->second, 1);
		
	}
	if (!upload_file->is_open())
		throw 404;
}

bool	Client::readRequest()
{
	char				buffer[200000];
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

// the uri must be decoded
			requestHandle.request_line.parse(string(_resBuff.begin(),
										   _resBuff.begin() + crlf_index));
			cout << "uri 1: " << requestHandle.request_line.getURI() << '\n';
			_resBuff.assign(_resBuff.begin() + crlf_index + 2, _resBuff.end());
			location_config = findLocation();
			it = find(location_config.methods.begin(), location_config.methods.end(),
			 requestHandle.request_line.getMethod());
			if (!location_config.redirectExist && it == location_config.methods.end())
				throw 403;
			requestHandle.request_line.removeDupSl();
			cout << "uri 2: " << requestHandle.request_line.getURI() << '\n';
			requestHandle.request_line.removeDotSegments();
			cout << "uri 3: " << requestHandle.request_line.getURI() << '\n';
			requestHandle.request_line.rootingPath(location_config.path, location_config.root, _serverConfig.root);
			cout << " => uri after: " << requestHandle.request_line.getURI() << '\n';
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
				if (std::strtoul(it->second.c_str(), NULL, 0) > _serverConfig.max_body_size)
					throw 400;
			if (location_config.redirectExist)
				return (true);
			if (!location_config.cgi.size() && requestHandle.request_line.getMethod() == "POST")
				setState(READ_BODY), postInit();
// handle if the CGI must be runned with a request that hasn't a body
			else if (location_config.cgi.size())
				cout << "";
			// setState(READ_BODY);
		}
		else
			return (false);
	}
	if (getState() == READ_BODY && upload_file)
	{
		unsigned long	size_tmp;

		if (string(_resBuff.begin(), _resBuff.end()).size() + contentSize >= content_length)
		{
			size_tmp = (string(_resBuff.begin(), _resBuff.end()).size() + contentSize) - content_length; 
			*upload_file << string(_resBuff.begin(), _resBuff.end() - size_tmp);
			upload_file->flush(), _resBuff.clear();
		}
		else
		{
			*upload_file << string(_resBuff.begin(), _resBuff.end());
			contentSize += string(_resBuff.begin(), _resBuff.end()).size();
			upload_file->flush(), _resBuff.clear();
			return (false);
		}
		// unsigned long written = static_cast<unsigned long>(upload_file->tellp());
		// if (written < content_length)
		// 	return (false);
	}
	return (true);
}

void	Client::processRequest()
{
	cout << "<<< processing request\n";
	if (isCGIRequest(requestHandle.request_line.getURI()))
	{
		fileName = startCGI();
		fileFd = open(fileName.c_str(), O_RDONLY);
		if (-1 == fileFd)
			throw 500;
		cout << "<<< filename: " << fileName << '\n';
		cout << "<<< state sated to CGI_HEADERS_WRITING\n";
		setState(CGI_HEADERS_WRITING);
		_byteSent = 0;
		return ;
	}
	else if (location_config.redirectExist)
	{
		Response			response;
		string				location;
		map<const string, const string>::const_iterator	it;

		it = requestHandle.request_header.getHeaderData().find("Host");
		if (it != requestHandle.request_header.getHeaderData().end())
			location = it->second;
		else
		{
			std::stringstream	ss;

			ss << _serverPort;
			location = _serverHost + ":" + ss.str();
		}
		cout << "http redirection: " << "http://" + location + location_config.redirect.url << '\n';
		if (location_config.redirect.status_code == 301)
			response.statusCode301("http://" + location
					+ location_config.redirect.url);
		else if (location_config.redirect.status_code == 302)
			response.statusCode302("http://" + location
					+ location_config.redirect.url);
		else
			throw 501;
		_resRes = response.getResponse();
	}
	else if (requestHandle.request_line.getMethod() != "POST")
	{
		Response	response(requestHandle, location_config);

		if (!requestHandle.request_line.getMethod().compare("DELETE"))
			response.DELETEResource();
		else if (!requestHandle.request_line.getMethod().compare("GET"))
			response.GETResource();
		_resRes += response.getResponse();
	}
	else if (requestHandle.request_line.getMethod() == "POST")
		_resRes = "HTTP/1.0 201 Created\r\n\r\n";
	_byteSent = 0;
 	setState(WRITING);
}

bool	Client::writeResponse()
{
	while (_byteSent < _resRes.size())
	{
		ssize_t bytesSent = send(_fd, _resRes.c_str() + _byteSent, _resRes.size() - _byteSent, 0);
		if (bytesSent < 0)
		{
			// std::cout << "send() error: 1" << strerror(errno) << "\n";
			return false;
		}
		_byteSent += bytesSent;
	}
	setState(DONE);
	return true;
}

bool	Client::writeCGIResponse()
{
	// std::cout << "\n\nstttttaaaate =>  " << _cgi->getState() << "\n\n";
	// std::cout << "   CGI state => " << _cgi->getState() << "\n";
	if (_cgi->getState() == CGI::CGI_RUNNING)
	{
		int status;
		pid_t result = waitpid(_cgi->getPid(), &status, WNOHANG);
	
		if (result == -1)
		{
			std::cerr << "waitpid error: " << strerror(errno) << std::endl;
			kill(_cgi->getPid(), SIGKILL);
			waitpid(_cgi->getPid(), NULL, WNOHANG);
			close(_cgi->getOutFile());
			return false;
		}
		else if (result == 0)
		{
			// Process still running but no data
			// This is OK, just wait
			return false;
		}
		else // result == pid (process finished)
		{
			// std::cout << "CGI process finished (PID " << result << ")" << std::endl;
			if (WIFEXITED(status))
			{
				// Normal exit
				int exitCode = WEXITSTATUS(status);
				std::cout << "  Exit code: " << exitCode << std::endl;
				if (exitCode != 0)
				{
					std::cerr << "  CGI exited with error code " << exitCode << std::endl;
					// Could send 500 error here
				}
			}
			close(_cgi->getOutFile());
			_cgi->setState(CGI::CGI_DONE);
			// setState(Client::CGI_WRITING);
		}
	}
	// std::cout << "   CGI state => " << _cgi->getState() << "\n";

	ssize_t	bytesSent;
	char	buffer[4096];

	    if (getState() == CGI_HEADERS_WRITING)
    {
        RequestHeader    response_header;
        size_t            crlf_index;
        setState(CGI_WRITING);
		
        bytesSent = read(fileFd, buffer, sizeof(buffer) - 1);
        string    cgi_body(buffer);
    
        crlf_index = cgi_body.find("\r\n\r\n", 0);
        if (crlf_index != string::npos)
        {
			response_header.parse(string(cgi_body.begin(), cgi_body.begin() + crlf_index + 2));
            send(_fd, "HTTP/1.0 200 OK\r\n", 17, 0);
            send(_fd, buffer, bytesSent, 0);
            cout << "<-<-<- HTTP/1.0 200 OK\r\n";
            cout << buffer;
        }
		else
		{
			std::ifstream is;
			is.open(fileName.c_str(), std::ios::binary);
			is.seekg(0, std::ios::end);
			_outputLength = is.tellg();
			is.close();
			std::ostringstream oss;
			oss << _outputLength;
			string response("HTTP/1.1 200 OK\r\nContent-Length: " + oss.str() + "\r\n\r\n");
		
			if (send(_fd, response.c_str(), response.size(), 0) < 0)
			{
				// std::cout << "send() error: 2" << strerror(errno) << "\n";
				return false;
			}
			bytesSent = send(_fd, buffer, bytesSent, 0);
		}
    }
	bytesSent = read(fileFd, buffer, sizeof(buffer) -1);
	if (bytesSent == -1)
		throw 500;
	buffer[bytesSent] = '\0';
	// close(fd);
	cout << "\\\\\\reading " << bytesSent << " from " << fileName << '\n';
	cout << '|' << buffer << "|\n";
	if (!bytesSent)
	{
		cout << "\\\\\\eof reached\n";
		return (setState(DONE), close(fileFd) ,true);
	}
	bytesSent = send(_fd, buffer, bytesSent, 0);
	if (bytesSent < 0)
	{
		// std::cout << "send() error: 3 " << strerror(errno) << "\n";
		return false;
	}
	return false;
}

// even the normal error files call must be http redirection
int	Client::writeErrorResponseHeaders()
{
	struct stat	st;
	Response	response;
	string		file_name;

	file_name = _serverConfig.root
		+ ((*_serverConfig.root.rbegin() != '/' && *fileName.begin() != '/') ? "/" : "") + fileName;
// look for the reason
	if (stat(file_name.c_str(), &st) == -1)
		throw std::runtime_error("cant get error page stat");
	response.statusCode301(fileName);
	if (-1 == send(_fd, response.getResponse().c_str(), response.getResponse().size(), 0))
		throw std::runtime_error("send fail");
	return (setState(DONE), 1);
}

// make this buffer larger
bool	Client::writeErrorResponse()
{
	char	buffer[4096];
	ssize_t	bytesSent;

	if (getState() == ERROR_HEADERS_WRITING)
		if (writeErrorResponseHeaders())
			return (true);
	bytesSent = read(fileFd, buffer, 4095);
	cout << "<+ <+ reading " << bytesSent << " of error page bytes " << fileFd << "\n";
// throw an exception that result to printing error in terminal
	if (bytesSent == -1)
		(void)buffer;
	if (!bytesSent)
	{
		cout << "<+ <+ done\n";
		setState(DONE);
		close(fileFd);
		return (true);
	}
	bytesSent = send(_fd, buffer, bytesSent, 0);
	return (false);
}

// this function must be modified the way it checks for CGI
bool	Client::isCGIRequest(const std::string &path)
{
	size_t	extension_index;

	extension_index = path.rfind('.', path.size());
	if (extension_index == string::npos)
		return (cout << "<<< it's not CGI\n", false);
	for (vector<CGIConfig>::iterator it = location_config.cgi.begin(); it != location_config.cgi.end(); it++)
		if (string(path.begin() + extension_index, path.end()) == it->extension)
			return (cout << "<<< it's CGI\n", true);
	return (cout << "<<< it's not CGI\n", false);
}

std::string	Client::startCGI()
{
	std::string scriptPath = requestHandle.request_line.getURI();

	if (access(scriptPath.c_str(), F_OK) != 0)
		throw 404;
	_cgi = new CGI();

	std::map<std::string, LocationConfig> lc = getServerConfig().locations;
	for (std::map<std::string, LocationConfig>::iterator it = lc.begin(); 
			it != lc.end(); it++)
	{
		LocationConfig lc_it = it->second;
		if (lc_it.hasCGI)
		{
			_cgi->cgi_c = lc_it.cgi;
		}
	}
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
		delete _cgi;
		_cgi = NULL;
		throw 500;
	}
}

void	Client::setServerInfo(const std::string &host, int port)
{
	_serverHost = host;
	_serverPort = port;
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
