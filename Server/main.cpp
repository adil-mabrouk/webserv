#include "Server.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include "../cgi/cgi.hpp"

volatile sig_atomic_t g_running = 1;

void signalHandler(int signum)
{
	(void)signum;
	std::cout << "\n[SIGNAL] Shutdown requested" << std::endl;
	g_running = 0;
}

int main()
{
	CGI cgi;
    cgi.setScriptPath("/home/amabrouk/Desktop/webserv/test.py");
    cgi.setMethod("GET");
    cgi.setQueryString("name=Alice");

    
    std::string response = cgi.execute();
	std::cout << response << std::endl;
}