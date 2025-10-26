#include "Server.hpp"

Server::Server() {}

void Server::initSocket(int port)
{
    // Create socket
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        std::string error = "socket() failed: ";
        error += strerror(errno);
        throw std::runtime_error(error);
    }
    
    // Set SO_REUSEADDR
    int opt = 1;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(listenFd);
        std::string error = "setsockopt() failed: ";
        error += strerror(errno);
        throw std::runtime_error(error);
    }
    
    // Make non-blocking
    int flags = fcntl(listenFd, F_GETFL, 0);
    if (flags < 0)
    {
        close(listenFd);
        std::string error = "fcntl(F_GETFL) failed: ";
        error += strerror(errno);
        throw std::runtime_error(error);
    }
    
    if (fcntl(listenFd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        close(listenFd);
        std::string error = "fcntl(F_SETFL) failed: ";
        error += strerror(errno);
        throw std::runtime_error(error);
    }
    
    // Bind to specific port
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));  // Zero out structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(listenFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        close(listenFd);
        std::string error = "bind() failed on port ";
        std::ostringstream oss;
        oss << port << ": " << strerror(errno);
        error += oss.str();
        throw std::runtime_error(error);
    }
    
    // Listen
    if (listen(listenFd, SOMAXCONN) < 0)
    {
        close(listenFd);
        std::string error = "listen() failed: ";
        error += strerror(errno);
        throw std::runtime_error(error);
    }
    
    // Success - add to vector
    _listenFds.push_back(listenFd);
    
    std::cout << "Server listening on port " << port << std::endl;
}

void	Server::handleNewConnection()
{
	// Accept new connection
	struct sockaddr_in clientAddress;
	socklen_t clientAddrLen = sizeof(clientAddress);
	Client	*client;
	int clientFd = accept(_listenFd, (struct sockaddr *)&clientAddress, &clientAddrLen);
	if (clientFd < 0)
	{
		perror("accept");
		return;
	}
	// Create a new Client object and store it in the clients map
	Client* newClient = new Client(clientFd);
	clients[clientFd] = newClient;
}

void Server::initPoller()
{
	struct pollfd fds[1];
	fds[0].fd = _listenFd;
	fds[0].events = POLLIN;
	for (;;)
	{
		int activity = poll(fds, 1, -1); // Wait indefinitely
		if (activity < 0)
		{
			perror("poll");
			return;
		}
		// Check for new connections
		if (fds[0].revents & POLLIN)
		{
			handleNewConnection();
		}
		// Additional logic to handle client read/write would go here
		char buffer[1024];
		int bytes = recv(_listenFd, buffer, sizeof(buffer), MSG_PEEK);
		if (bytes > 0)
		{
			// Handle client read
			buffer[bytes] = 0;
			printf("Received data: %s\n", buffer);
		}
		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
		send(_listenFd, response.c_str(), response.size(), 0);
		if (fds[0].revents & POLLIN) {
			recv(fds[0].fd, buffer, sizeof(buffer), 0);
		}
		if (fds[0].revents & POLLOUT) {
			send(fds[0].fd, response.c_str(), response.size(), 0);
		}
		close(clients[_listenFd]->getFd());
		fds[0].fd = -1;  // remove from poll array

	}
}

bool	Server::isListeningSocket(int fd) const
{
	for (size_t i = 0; i < _listenFds.size(); i++)
	{
		if (_listenFds[i] == fd)
			return 1;
	}
	return 0;
}

void Server::run()
{
    // BEFORE loop: Initialize from config
    // Config config = parseConfig();
    // std::vector<int> ports = config.getPorts();
    
    // for (size_t i = 0; i < ports.size(); i++)
    //     initSocket(ports[i]);  // Creates multiple listening sockets

	initSocket(8080); // Example port before config integration
    // Main event loop
    while (true)
    {
        // Build poll array (listening sockets + clients)
        std::vector<struct pollfd> pollFds;
        
        // Add ALL listening sockets
        for (size_t i = 0; i < _listenFds.size(); i++)
        {
            struct pollfd pfd;
            pfd.fd = _listenFds[i];
            pfd.events = POLLIN;
            pfd.revents = 0;
            pollFds.push_back(pfd);
        }
        
        // Add all client sockets
        // ... (your code here is OK)
        
        // Wait with timeout
        int activity = poll(&pollFds[0], pollFds.size(), 1000);
        
        if (activity < 0)
        {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
        
        // if (activity == 0)  // Timeout
        // {
        //     checkTimeouts();
        //     continue;
        // }
        
        // Process events
        for (size_t i = 0; i < pollFds.size(); i++)
        {
            if (pollFds[i].revents == 0)
                continue;
            
            int fd = pollFds[i].fd;
            
            // Check if ANY listening socket
            if (isListeningSocket(fd))
            {
                if (pollFds[i].revents & POLLIN)
                    handleNewConnection();
            }
            else  // Client socket
            {
                // ... (your code here is mostly OK)
            }
        }
        
        // Cleanup
        std::vector<int> toClose;
        for (auto it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second->isDone())  // Better check
                toClose.push_back(it->first);
        }
        
        for (size_t i = 0; i < toClose.size(); i++)
            closeClient(toClose[i]);
    }
}