#include "Server.hpp"

static bool g_running = true;

static void signalHandler(int signo)
{
	std::cout << "\nReceived signal " << signo << ", shutting down...\n";
	g_running = false;
}


Server::Server(const std::vector<ServerConfig> &configs) : _serverConfigs(configs) {
	for (size_t i = 0; i < _serverConfigs.size(); i++) {
		for (size_t j = 0; j < _serverConfigs[i].listenList.size(); j++) {
			std::string host = _serverConfigs[i].listenList[j].first;
			int port = _serverConfigs[i].listenList[j].second;
			initSocket(host, port, i);
		}
	}
}

static void	throwError(int fd, std::string error)
{
	close(fd);
	throw std::runtime_error(error + strerror(errno));
}

void	Server::initSocket(std::string host, int port, size_t configIndex)
{
	int	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFd < 0)
	throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
	int opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throwError(listenFd, "setsockopt() failed: ");
	
	int f = fcntl(listenFd, F_GETFL, 0);
	if (f < 0)
		throwError(listenFd, "fcntl(F_GETFL) failed: ");

	if (fcntl(listenFd, F_SETFL, f | O_NONBLOCK) < 0)
		throwError(listenFd, "fcntl(F_SETFL) failed: ");

	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(listenFd, (struct sockaddr*)&address, sizeof(address)) < 0)
		throwError(listenFd, "bind() failed: ");
	
	if (listen(listenFd, SOMAXCONN) < 0)
		throwError(listenFd, "listen() failed: ");

	_listenFds.push_back(listenFd);
	std::cout << "Server listening on port " << port << std::endl;
	
	_fdToConfigIndex[listenFd] = configIndex;
	std::cout << "Server listening on " << host << ":" << port << std::endl;
}

short	Server::determineClientEvents(Client* clt)
{
	short	events = 0;
	if (clt->getState() == Client::READING)
		events |= POLLIN;
	else if (clt->getState() == Client::WRITING)
		events |= POLLOUT;
	return events;
}

bool	Server::isListeningSocket(int fd) const
{
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		if (_listenFds[i] == fd)
			return true;
	}
	return false;
}

ServerConfig	*Server::getConfigForListenFd(int fd) {
	std::map<int, size_t>::iterator it = _fdToConfigIndex.find(fd);
	if (it != _fdToConfigIndex.end())
			return &_serverConfigs[it->second];
	return NULL;
}

void	Server::handleNewConnection(int fd)
{
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen = sizeof(clientAddr);

	int	clientFd = accept(fd, (struct sockaddr*)&clientAddr, &addrLen);
	// std::cout << "================================fd accepted: " << clientFd << std::endl;
	if (clientFd < 0)
	{
		std::cerr << "accept() error: " << strerror(errno) << std::endl;
		return;
	}

	int f = fcntl(clientFd, F_GETFL, 0);
	if (f < 0 || fcntl(clientFd, F_SETFL, f | O_NONBLOCK) < 0)
		throwError(clientFd, "Failed to set non-blocking: ");
	ServerConfig	*config = getConfigForListenFd(fd);
	// fcntl(clientFd, F_SETFD, FD_CLOEXEC);
	Client	*client = new Client(clientFd, config);
	_clients[clientFd] = client;
	// std::cout << "New client connected fd = " << clientFd << ")\n";
}

void	Server::closeClient(int fd)
{
	std::map<int, Client*>::iterator	it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	// std::cout << "closing client fd = " << fd << "\n";
	delete it->second;
	_clients.erase(it);
	close(fd);
}

void	Server::handleClientRead(int clientFd)
{
	Client	*client = _clients[clientFd];
	if (!client)
		return ;
	bool	complete = client->readRequest();
	if (client->getState() == Client::DONE)
		return ;
	if (complete)
	{
		// std::cout << "Request complete from fd = " << clientFd << "\n";
		client->processRequest();
	}
}

void	Server::handleClientWrite(int clientFd)
{
	Client	*client = _clients[clientFd];
	if (!client)
		return ;
	bool	complete = client->writeResponse();
	if (complete)
	{
		// std::cout << "Response sent to Fd = " << clientFd << "\n";
		closeClient(clientFd);
	}
}

/*
This function implements a non-blocking, single-threaded event loop that:
- Manages multiple listening sockets (ports)
- Accepts new client connections
- Handles client read/write operations
- Cleans up finished connections
*/

void	Server::run()
{
	signal(SIGINT, signalHandler);   // Ctrl+C
	signal(SIGTERM, signalHandler);  // kill command
	while (g_running) // main event loop
	{
		std::vector<struct pollfd>	pollFds; // Array of pollfd structures for poll()
		for (size_t i = 0; i < _listenFds.size(); i++) // Add all listening sockets
		{
			struct pollfd pfd; //The struct pollfd is a data structure used to monitor file descriptors for I/O events in the poll() system call
			pfd.fd = _listenFds[i];
			pfd.events = POLLIN; // We are interested in read events (incoming connections)
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++) // Add all client sockets
		{
			struct pollfd	pfd;
			pfd.fd = it->first;
			pfd.events = determineClientEvents(it->second); // Determine if we want to read or write
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		int activity = poll(&pollFds[0], pollFds.size(), 1000); // waits for one of a set of file descriptors to become ready to perform I/O.
		if (activity < 0)
		{
			std::cerr << "poll() error: " << strerror(errno) << "\n";
			continue;
		}
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			if (pollFds[i].revents == 0)
				continue ; 
			int fd = pollFds[i].fd;
			short revents = pollFds[i].revents;
			if (isListeningSocket(fd))
			{
				if (revents & POLLIN)
					handleNewConnection(fd);
			}
			else
			{
				if (revents & POLLIN)
					handleClientRead(fd);
				if (revents & POLLOUT)
					handleClientWrite(fd);
			}
		}
		std::vector<int>	clientsToClose; // Collect clients to close after iteration
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			if (it->second->getState() == Client::DONE)
				clientsToClose.push_back(it->first);
		}
		for (size_t i = 0; i < clientsToClose.size(); i++)
			closeClient(clientsToClose[i]);
	}
	// cleanup on shutdown and close all connections
	std::cout << "\n\nCleaning up ...";
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
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