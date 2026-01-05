#include "Server.hpp"

static bool g_running = true;

static void signalHandler(int signo)
{
	std::cout << "\nReceived signal " << signo << ", shutting down...\n";
	g_running = false;
}

Server::Server(const std::vector<ServerConfig> &configs) : _serverConfigs(configs)
{
	for (size_t i = 0; i < _serverConfigs.size(); i++)
	{
		for (size_t j = 0; j < _serverConfigs[i].listenList.size(); j++)
		{
			std::string host = _serverConfigs[i].listenList[j].first;
			int port = _serverConfigs[i].listenList[j].second;
			initSocket(host, port, i);
		}
	}
}

static void throwError(int fd, std::string error)
{
	close(fd);
	fd = -1;
	throw std::runtime_error(error + strerror(errno));
}

void Server::initSocket(std::string host, int port, size_t configIndex)
{
	struct addrinfo hints, *res;
	int listenFd = -1;
	int status;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	std::ostringstream portStr;
	portStr << port;

	status = getaddrinfo(host.c_str(), portStr.str().c_str(), &hints, &res);
	if (status != 0)
		freeaddrinfo(res), throw std::runtime_error("getaddrinfo() failed");
	listenFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (listenFd < 0)
		freeaddrinfo(res), throw std::runtime_error("socket() failed: "
											  + std::string(strerror(errno)));
	int opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		freeaddrinfo(res), throwError(listenFd, "setsockopt() failed: ");
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0)
		freeaddrinfo(res), throwError(listenFd, "fcntl(F_SETFL) failed: ");
	if (fcntl(listenFd, F_SETFD, FD_CLOEXEC) < 0)
		freeaddrinfo(res), throwError(listenFd, "fcntl(F_SETFD) failed: ");
	if (bind(listenFd, res->ai_addr, res->ai_addrlen) < 0)
		freeaddrinfo(res), throwError(listenFd, "bind() failed: "); // freeaddrinfo(res);
	freeaddrinfo(res);
	if (listen(listenFd, SOMAXCONN) < 0)
		throwError(listenFd, "listen() failed: ");
	_listenFds.push_back(listenFd);
	_fdToConfigIndex[listenFd] = configIndex;
	std::cout << "Server listening on " << host << ":" << port << std::endl;
}

short Server::determineClientEvents(Client *clt)
{
	short events = 0;
	if (clt->getState() == Client::WRITING
			|| clt->getState() == Client::CGI_WRITING
			|| clt->getState() == Client::CGI_RUNNING
			|| clt->getState() == Client::CGI_HEADERS_WRITING
			|| clt->getState() == Client::ERROR_HEADERS_WRITING
			|| clt->getState() == Client::ERROR_WRITING)
		events |= POLLOUT;
	else
		events |= POLLIN;
	return events;
}

bool Server::isListeningSocket(int fd) const
{
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		if (_listenFds[i] == fd)
			return true;
	}
	return false;
}

ServerConfig Server::getConfigForListenFd(int fd)
{
	std::map<int, size_t>::iterator it = _fdToConfigIndex.find(fd);
	if (it != _fdToConfigIndex.end())
		return _serverConfigs[it->second];
	return ServerConfig();
}

void Server::handleNewConnection(int fd)
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	int clientFd = accept(fd, (struct sockaddr *)&clientAddr, &addrLen);
	std::cout << "fd server = " << fd << "\n";
	std::cout << "fd accepted = " << clientFd << "\n";
	if (clientFd < 0)
		return;
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
		throwError(clientFd, "Failed to set non-blocking: ");
	if (fcntl(clientFd, F_SETFD, FD_CLOEXEC) < 0)
		throwError(clientFd, "fcntl(F_SETFD) failed: ");

	ServerConfig config = getConfigForListenFd(fd);
	Client *client = new Client(clientFd, config);
	if (!client)
		throwError(clientFd, "Failed to allocate memory for new client");
	_clients[clientFd] = client;
	std::cout << "New client connected fd = " << clientFd << ")\n";
}

void Server::closeClient(int fd)
{
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;
	std::cout << "closing client fd => " << fd << "\n";
	close(fd);
	fd = -1;
	delete it->second;
	_clients.erase(it);
}

void Server::handleClientRead(int clientFd)
{
	Client *client = _clients[clientFd];
	if (!client)
		return;
	bool complete = client->readRequest();
	if (client->getState() == Client::DONE)
	{
		// Never going throug there
		cout << "# # # client done------------------------------------------------------------------------------------------------------\n";
		return;
	}
	if (complete)
		cout << "+ + + processing request\n",
		client->processRequest();
}

void Server::handleClientWrite(int clientFd)
{
	bool	complete;

	Client *client = _clients[clientFd];
	if (!client)
		return;
	if (client->getState() == Client::CGI_WRITING
		|| client->getState() == Client::CGI_HEADERS_WRITING)
		// cout << "+ + + writing cgi response\n",
		complete = client->writeCGIResponse();
	else if (client->getState() == Client::ERROR_WRITING
			|| client->getState() == Client::ERROR_HEADERS_WRITING)
		cout << "+ + + writing error response\n",
		complete = client->writeErrorResponse();
	else
		cout << "+ + + writing response\n",
		complete = client->writeResponse();
	if (complete)
	{
		if (client->getCGI())
		{
			std::cout << "CGI\n";
			closeClient(clientFd);
		}
		else
		{
			std::cout << "Client\n";
			closeClient(clientFd);
		}
		
	}
}

void	Server::handleClientError(Client* client, int status)
{
	map<int, std::string>::const_iterator	it;
	string									root;

	root = client->getServerConfig().root;
	it = client->getServerConfig().error_pages.find(status);
	client->setFileFd(-1);
	if (it != client->getServerConfig().error_pages.end())
	{
		string	file_name;

		file_name = root
			+ ((*root.rbegin() != '/' && *it->second.begin() != '/') ? "/" : "") + it->second;
		cout << "+ + + error page: " << file_name << "\n";
		client->setState(Client::ERROR_HEADERS_WRITING);
		client->setFileFd(open(file_name.c_str(), O_RDONLY));
		client->setFileName(it->second.c_str());
		client->setErrorStatus(status);
	}
	if (client->getFileFd() == -1)
	{
		cout << "+ + + normal pages " << status << '\n';
		Response res;

		client->setState(Client::WRITING);
		switch (status)
		{
			case 400:
				res.statusCode400();
				break;
			case 401:
				res.statusCode401();
				break;
			case 403:
				res.statusCode403();
				break;
			case 404:
				res.statusCode404();
				break;
			case 500:
				res.statusCode500();
				break;
			case 501:
				res.statusCode501();
				break;
		}
		client->_resRes = res.getResponse();
	}

}

void Server::run()
{
	int activity;

	signal(SIGINT, signalHandler); // Ctrl+C
	// signal(SIGTERM, signalHandler);  // kill command
	while (g_running) // main event loop
	{
		std::vector<struct pollfd> pollFds;			   // Array of pollfd structures for poll()
		for (size_t i = 0; i < _listenFds.size(); i++) // Add all listening sockets
		{
			struct pollfd pfd; // The struct pollfd is a data structure used to monitor file descriptors for I/O events in the poll() system call
			pfd.fd = _listenFds[i];
			pfd.events = POLLIN; // We are interested in read events (incoming connections)
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++) // Add all client sockets
		{
			struct pollfd pfd;
			pfd.fd = it->first;
			pfd.events = determineClientEvents(it->second); // Determine if we want to read or write
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		activity = poll(&pollFds[0], pollFds.size(), 1000); // waits for one of a set of file descriptors to become ready to perform I/O.
		if (activity < 0)
			continue;

		for (size_t i = 0; i < pollFds.size(); i++)
		{
			int fd = pollFds[i].fd;
			short revents = pollFds[i].revents;
			
			if (pollFds[i].revents == 0)
				continue;
			if (isListeningSocket(fd))
			{
				if (revents & POLLIN)
					cout << "\n\nnew connection\n",
					handleNewConnection(fd);
			}
			else
			{
				try
				{
					if (revents & POLLIN)
						cout << "- - - reading\n",
						handleClientRead(fd);
					if (revents & POLLOUT)
						// cout << "+ + + writing\n",
						handleClientWrite(fd);
					if (_clients.find(fd) != _clients.end())
						checkCGITimeouts(_clients.find(fd)->second);
				}
				catch (int &status)
				{
					handleClientError(_clients.find(fd)->second, status);
				}
			}
		}
	}
	// cleanup on shutdown and close all connections
	std::cout << "\n\nCleaning up ...";
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		std::cout << "Closing client fd = " << it->first << "\n";
		delete it->second;
	}
	_clients.clear();
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		std::cout << "Closing listening socket fd = " << _listenFds[i] << "\n";
		close(_listenFds[i]);
		_listenFds[i] = -1;
	}
	_listenFds.clear();
	std::cout << "Cleanup completed" << "\n";
}

void Server::checkCGITimeouts(Client* ctl)
{
	time_t now = std::time(NULL);

	if (ctl->getCGI() && now - ctl->getCGI()->getStartTime() > 3)
	{
		std::cout << "CGI timeout\n";
		killCGI(ctl);

		throw 500;
	}
}

void Server::killCGI(Client *client)
{
	CGI *cgi = client->getCGI();
	if (!cgi)
		return;

	kill(cgi->getPid(), SIGKILL);
	waitpid(cgi->getPid(), NULL, WNOHANG);
}
