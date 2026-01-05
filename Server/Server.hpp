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
#include <arpa/inet.h>
#include "../Configuration/config.hpp"
#include <sys/wait.h>
#include "../CGI/cgi.hpp"
#include <netdb.h>

class Client;

class Server {
	private:
		std::vector<ServerConfig>		_serverConfigs;  
		std::map<int, size_t>			_fdToConfigIndex;
		std::vector<int>				_listenFds;  
		std::map<int, Client*>			_clients;  
	public:
		Server(const std::vector<ServerConfig> &configs);
		 
		void							run();  
		void							initSocket(std::string host, int port, size_t configIndex);  
		ServerConfig					getConfigForListenFd(int fd);
		short							determineClientEvents(Client *clt);  
		bool							isListeningSocket(int fd) const;  
		void							handleNewConnection(int fd);  
		void							handleClientRead(int ClientFd);  
		int							handleClientWrite(int clientFd);  
		void							closeClient(int clientFd);  

		void			checkCGITimeouts(int fd);
		void			killCGI(Client* client);
		~Server(){}
};
