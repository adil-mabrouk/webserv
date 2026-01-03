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
	throw std::runtime_error(error + strerror(errno));
}

void Server::initSocket(std::string host, int port, size_t configIndex)
{
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFd < 0)
		throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
	int opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throwError(listenFd, "setsockopt() failed: ");

	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0)
		throwError(listenFd, "fcntl(F_SETFL) failed: ");
	if (fcntl(listenFd, F_SETFD, FD_CLOEXEC) < 0)
		throwError(listenFd, "fcntl(F_SETFD) failed: ");

	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(host.c_str());
	address.sin_port = htons(port);

	if (bind(listenFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throwError(listenFd, "bind() failed: ");

	if (listen(listenFd, SOMAXCONN) < 0)
		throwError(listenFd, "listen() failed: ");

	_listenFds.push_back(listenFd);
	// std::cout << "Server listening on port " << port << std::endl;

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
	{
		std::cerr << "accept() error: " << strerror(errno) << std::endl;
		return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
		throwError(clientFd, "Failed to set non-blocking: ");
	if (fcntl(clientFd, F_SETFD, FD_CLOEXEC) < 0)
		throwError(clientFd, "fcntl(F_SETFD) failed: ");

	ServerConfig config = getConfigForListenFd(fd);
	Client *client = new Client(clientFd, config);
	// protect
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
		cout << "# # # client done\n";
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

/*
This function implements a non-blocking, single-threaded event loop that:
- Manages multiple listening sockets (ports)
- Accepts new client connections
- Handles client read/write operations
- Cleans up finished connections
*/

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
		{
			std::cerr << "poll() error: " << strerror(errno) << "\n";
			continue;
		}

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
				}
				catch (int &status)
				{
					map<int, std::string>::const_iterator	it;
					string									root;

					root = _clients.find(fd)->second->getServerConfig().root;
					it = _clients.find(fd)->second->getServerConfig().error_pages.find(status);
					_clients.find(fd)->second->setFileFd(-1);
					if (it != _clients.find(fd)->second->getServerConfig().error_pages.end())
					{
						string	file_name;

						file_name = root
							+ ((*root.rbegin() != '/' && *it->second.begin() != '/') ? "/" : "") + it->second;
						cout << "+ + + error page: " << file_name << "\n";
						_clients.find(fd)->second->setState(Client::ERROR_HEADERS_WRITING);
						_clients.find(fd)->second->setFileFd(open(file_name.c_str(), O_RDONLY));
						_clients.find(fd)->second->setFileName(it->second.c_str());
						_clients.find(fd)->second->setErrorStatus(status);
					}
					if (_clients.find(fd)->second->getFileFd() == -1)
					{
						cout << "+ + + normal pages\n";
						Response res;

						_clients.find(fd)->second->setState(Client::WRITING);
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
						_clients.find(fd)->second->_resRes = res.getResponse();
					}
					// cout << "+ + + writing\n";
					handleClientWrite(fd);
				}
			}
		}

		checkCGITimeouts();

	}
	// cleanup on shutdown and close all connections
	std::cout << "\n\nCleaning up ...";
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		std::cout << "Closing client fd = " << it->first << "\n";
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		std::cout << "Closing listening socket fd = " << _listenFds[i] << "\n";
		close(_listenFds[i]);
	}
	_listenFds.clear();
	std::cout << "Cleanup completed" << "\n";
}

void Server::checkCGITimeouts()
{
	time_t now = time(NULL);

	for (std::map<int, Client *>::iterator it = _clients.begin();
		 it != _clients.end(); it++)
	{
		if (it->second->getCGI())
		{
			// std::cout << "state timeout => " << it->second->getState() << "\n";
			if (now - it->second->getCGI()->getStartTime() > 3)
			{
				std::cout << "CGI timeout\n";
				killCGI(it->second);

				std::cerr << "HTTP/1.0 504 Gateway Timeout\r\n\r\n";
				// it->second->setState(Client::WRITING);
			}
		}
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
