#include ""

void Server::initSocket(std::string host, int port, size_t configIndex)
{
	struct addrinfo hints, *res, *p;
	int listenFd = -1;
	int status;
	
	// Setup hints for getaddrinfo
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;      // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;  // TCP socket
	hints.ai_flags = AI_PASSIVE;      // Use my IP (for binding)
	
	// Convert port to string
	std::ostringstream portStr;
	portStr << port;
	
	// Resolve address
	status = getaddrinfo(host.c_str(), portStr.str().c_str(), &hints, &res);
	if (status != 0)
		throw std::runtime_error("getaddrinfo() failed: " + std::string(gai_strerror(status)));
	
	// Try each address until we successfully bind
	for (p = res; p != NULL; p = p->ai_next)
	{
		// Create socket
		listenFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenFd < 0)
			continue;  // Try next address
		
		// Set socket options
		int opt = 1;
		if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			close(listenFd);
			listenFd = -1;
			continue;
		}
		
		// Set non-blocking
		if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0)
		{
			close(listenFd);
			listenFd = -1;
			continue;
		}
		
		// Set close-on-exec
		if (fcntl(listenFd, F_SETFD, FD_CLOEXEC) < 0)
		{
			close(listenFd);
			listenFd = -1;
			continue;
		}
		
		// Bind
		if (bind(listenFd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listenFd);
			listenFd = -1;
			continue;  // Try next address
		}
		
		// Success! Break out of loop
		break;
	}
	
	// Free the address info list
	freeaddrinfo(res);
	
	// Check if we successfully bound
	if (listenFd < 0 || p == NULL)
		throw std::runtime_error("Failed to bind to " + host + ":" + portStr.str());
	
	// Listen
	if (listen(listenFd, SOMAXCONN) < 0)
		throwError(listenFd, "listen() failed: ");
	
	_listenFds.push_back(listenFd);
	_fdToConfigIndex[listenFd] = configIndex;
	
	std::cout << "Server listening on " << host << ":" << port << std::endl;
}