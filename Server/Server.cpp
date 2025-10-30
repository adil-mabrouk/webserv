#include "Server.hpp"

// static void signalHandler(int signo)
// {
// 	std::cout << "Shutting down server (signal " << signo << ")" << std::endl;
// 	std::exit(0);
// }

Server::Server() {}

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
		throwError(listenFd, "socket() failed: ");

	int opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throwError(listenFd, "setsockopt() failed: ");
	
	int f = fcntl(listenFd, F_GETFL, 0);
	if (f < 0);
		throwError(listenFd, "fcntl(F_GETFL) failed: ");

	if (fcntl(listenFd, F_SETFL, f | O_NONBLOCK) < 0)
		throwError(listenFd, "fcntl(F_SETFL) failed: ");

	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (listen(listenFd, SOMAXCONN) < 0)
		throwError(listenFd, "listen() failed: ");

	_listenFds.push_back(listenFd);
	std::cout << "Server listening on port " << port << std::endl;
}

short	Server::determineClientEvents(Client* clt)
{
	
}

void	checkTimeouts()
{

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

void	Server::handleNewConnection(int fd)
{
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen = sizeof(clientAddr);

	int	clientFd = accept(fd, (struct sockaddr*)&clientAddr, &addrLen);
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
		std::cout << "Request complete from fd = " << clientFd << "\n";
		client->processRequest();
	}
}

void	Server::run()
{
	int port = 8080;
	initSocket(port);
	// signal(SIGINT, signalHandler);   // Ctrl+C
	// signal(SIGTERM, signalHandler);  // kill command
	while (true)
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
		if (activity == 0)
		{
			checkTimeouts();
			continue ;
		}
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			if (pollFds[i].revents == 0)
				continue ;
			int fd = pollFds[i].fd;
			short revents = pollFds[i].revents;
			if (isListeningSocket(fd))
				if (revents & POLLIN)
					handleNewConnection(fd);
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
	}
}