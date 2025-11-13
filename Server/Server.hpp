#pragma once

#include <map>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <csignal>
#include <cerrno>
#include "Client.hpp"

class Client;

class Server {
	private:
		std::vector<int>		_ports; // List of ports the server listens on
		std::vector<int>		_listenFds; // List of listening socket file descriptors
		std::map<int, Client*>	_clients; // key: client socket fd, value: Client object pointer
		int						_timeoutSeconds;
	public:
		Server();
		void					run(); // Main event loop
		void					initSocket(int port); // Initialize a listening socket on the given port
		short					determineClientEvents(Client *clt); // Determine if we want to read or write
		// void					checkTimeouts();
		bool					isListeningSocket(int fd) const; // Check if fd is a listening socket
		void					handleNewConnection(int fd); // Accept new client connection
		void					handleClientRead(int ClientFd); // Handle reading from client
		void					handleClientWrite(int clientFd); // Handle writing to client
		void					closeClient(int clientFd); // Close and clean up client connection
		// void					setTimeoutSeconds(int seconds);
		// int						getTimeoutSeconds() const;
		void					checkTimeouts();
};