#include "Request/Request.hpp"
#include "Server/Server.hpp"
#include "Configuration/config.hpp"

int main(int ac, char** av)
{
	signal(SIGPIPE, SIG_IGN);
	
	if (ac != 2) {
		std::cerr << "Usage: ./webserv config_file\n";
		return 1;
	}

	try {
		ConfigParser				parser(av[1]);
		std::vector<ServerConfig>	configs = parser.parser();
		if (configs.empty()) {
			std::cerr << "No server configuration found\n";
			return 1;
		}
		std::cout << "╔══════════════════════════════════╗" << std::endl;
		std::cout << "║       Webserv HTTP Server        ║" << std::endl;
		std::cout << "║            Engine X              ║" << std::endl;
		std::cout << "╚══════════════════════════════════╝" << std::endl;
		std::cout << std::endl;
		Server	server(configs);
		server.run();
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "Error\n";
		return 1;
	}
	return 0;
}
