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
#include "Client.hpp"

class Server {
	private:
		std::vector<int>	_listenFds;
		std::map<int, Client*>	_clients;
	public:
		void	run();
		void	initSocket(int port);
		short	determineClientEvents(Client *clt);
		void	checkTimeouts();
		bool	isListeningSocket(int fd) const;
		void	handleNewConnection(int fd);
		void	handelClientRead(int fd);
};