#include "Server.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>

volatile sig_atomic_t g_running = 1;

void signalHandler(int signum)
{
	(void)signum;
	std::cout << "\n[SIGNAL] Shutdown requested" << std::endl;
	g_running = 0;
}

int main()
{
	std::cout << "╔══════════════════════════════════╗" << std::endl;
	std::cout << "║     Webserv HTTP Server          ║" << std::endl;
	std::cout << "║     C++98 Implementation         ║" << std::endl;
	std::cout << "╚══════════════════════════════════╝" << std::endl;
	std::cout << std::endl;

	try
	{
		Server server;

		// Run server
		server.run();
		
		std::cout << "\n" << std::string(40, '=') << std::endl;
		std::cout << "[STATUS] Server shutdown complete" << std::endl;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "\n[FATAL] Runtime error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "\n[FATAL] Exception: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "\n[FATAL] Unknown error occurred" << std::endl;
		return 1;
	}
	
	return 0;
}