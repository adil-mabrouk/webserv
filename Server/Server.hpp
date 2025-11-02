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
		std::vector<int>	_listenFds;
		std::map<int, Client*>	_clients;
	public:
		Server();
		void	run();
		void	initSocket(int port);
		short	determineClientEvents(Client *clt);
		// void	checkTimeouts();
		bool	isListeningSocket(int fd) const;
		void	handleNewConnection(int fd);
		void	handleClientRead(int ClientFd);
		void	closeClient(int clientFd);
		void	handleClientWrite(int clientFd);
};