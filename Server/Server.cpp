#include "Server.hpp"

static bool g_running = true;

static void signalHandler(int signo)
{
    std::cout << "\nReceived signal " << signo << ", shutting down...\n";
    g_running = false;
}


Server::Server() {}

static void	throwError(int fd, std::string error)
{
	close(fd);
	throw std::runtime_error(error + strerror(errno));
}

void	Server::initSocket(int port)
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

// void	checkTimeouts()
// {

// }

bool	Server::isListeningSocket(int fd) const
{
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		if (_listenFds[i] == fd)
			return true;
	}
	return false;
}

void	Server::handleNewConnection(int fd)
{
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen = sizeof(clientAddr);

	int	clientFd = accept(fd, (struct sockaddr*)&clientAddr, &addrLen);
	std::cout << "================================fd accepted: " << clientFd << std::endl;
	if (clientFd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		std::cerr << "accept() error: " << strerror(errno) << std::endl;
        return;
	}

	int f = fcntl(clientFd, F_GETFL, 0);
	if (f < 0 || fcntl(clientFd, F_SETFL, f | O_NONBLOCK) < 0)
		throwError(clientFd, "Failed to set non-blocking: ");
	// fcntl(clientFd, F_SETFD, FD_CLOEXEC);
	Client	*client = new Client(clientFd);
	_clients[clientFd] = client;
	std::cout << "New client connected fd = " << clientFd << ")\n";
}

void	Server::closeClient(int fd)
{
	std::map<int, Client*>::iterator	it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	std::cout << "closing client fd = " << fd << "\n";
	// delete it->second;
	// _clients.erase(it);
	// close(fd);
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
		std::cout << "Request complete from fd = " << clientFd << "\n";
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
		std::cout << "Response sent to Fd = " << clientFd << "\n";
		if (client->shouldkeepAlive())
			client->resetForNextRequest();
		// else
		// 	closeClient(clientFd);
	}
}

void	Server::run()
{
	_ports.push_back(9091);
	// _ports.push_back(9000);
	// _ports.push_back(4000);
	for (size_t i = 0; i < _ports.size(); i++)
		initSocket(_ports[i]);
	signal(SIGINT, signalHandler);   // Ctrl+C
	signal(SIGTERM, signalHandler);  // kill command
	while (g_running)
	{
		std::vector<struct pollfd>	pollFds;
		for (size_t i = 0; i < _listenFds.size(); i++)
		{
			struct pollfd pfd;
			pfd.fd = _listenFds[i];
			pfd.events = POLLIN;
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			struct pollfd	pfd;
			pfd.fd = it->first;
			pfd.events = determineClientEvents(it->second);
			pfd.revents = 0;
			pollFds.push_back(pfd);
		}
		int activity = poll(&pollFds[0], pollFds.size(), 1000);
		if (activity < 0)
		{
			if (errno == EINTR)
				continue ;
			std::cerr << "poll() error: " << strerror(errno) << "\n";
			break ;
		}
		// if (activity == 0)
		// {
		// 	checkTimeouts();
		// 	continue ;
		// }
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
				if (revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					closeClient(fd);
					continue ;
				}
				if (revents & POLLIN)
					handleClientRead(fd);
				if (revents & POLLOUT)
					handleClientWrite(fd);
			}
		}
		std::vector<int>	clientsToClose;
		for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			if (it->second->getState() == Client::DONE)
				clientsToClose.push_back(it->first);
		}
		for (size_t i = 0; i < clientsToClose.size(); i++)
			closeClient(clientsToClose[i]);
	}
	std::cout << "\n\nCleaning up ...";
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		// std::cout << "Closing client fd = " << it->first << "\n";
		// close(it->first);
		// delete it->second;
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