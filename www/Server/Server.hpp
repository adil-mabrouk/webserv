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

class Client;

class Server {
	private:
		std::vector<ServerConfig>		_serverConfigs; // All server blocks from config
		std::map<int, size_t>			_fdToConfigIndex;
		std::vector<int>				_listenFds; // List of listening socket file descriptors
		std::map<int, Client*>			_clients; // key: client socket fd, value: Client object pointer
	public:
		Server(const std::vector<ServerConfig> &configs);
		// ~Server();
		void							run(); // Main event loop
		void							initSocket(std::string host, int port, size_t configIndex); // Initialize a listening socket on the given ports
		ServerConfig					getConfigForListenFd(int fd);
		short							determineClientEvents(Client *clt); // Determine if we want to read or write
		bool							isListeningSocket(int fd) const; // Check if fd is a listening socket
		void							handleNewConnection(int fd); // Accept new client connection
		void							handleClientRead(int ClientFd); // Handle reading from client
		void							handleClientWrite(int clientFd); // Handle writing to client
		void							closeClient(int clientFd); // Close and clean up client connection


		Client*			findClientByCGIPipe(int pipeFd);
		void			handleCGIRead(Client* client);
		void			checkCGITimeouts();
		void			killCGI(Client* client);

};