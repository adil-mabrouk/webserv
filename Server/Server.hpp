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
#include "Client.hpp"

class Server {
	private:
		int						_listenFd;	 // listening socket
		std::vector<int>		_listenFds; // support multiple ports

		int						_pollFd;	 // epoll/kqueue/poll handle
		std::map<int, Client*>	clients;	// fd → Client object

	public:
		// Constructors / destructors
		Server();
		~Server();

		// Core setup
		void					initSocket(int port); // create / bind / listen
		void					initPoller();	// poll
		void					run();	 // main event loop

	private:
		// Internal helpers
		void					handleNewConnection();	// accept new client
		void					handleClientRead(int clientFd);	// read from client
		void					handleClientWrite(int clientFd);	// write to client
		void					closeClient(int clientFd);	// clean up client
		bool					isListeningSocket(int fd) const; // check if fd is a listening socket
};