#include "Server/Server.hpp"
#include "Configuration/config.hpp"

int main(int ac, char** av)
{
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
		std::cout << "Loaded " << configs.size() << " server block(s)\n";
		Server	server(configs);
		server.run();
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	// (void)ac;
	// std::cout << "╔══════════════════════════════════╗" << std::endl;
	// std::cout << "║     Webserv HTTP Server          ║" << std::endl;
	// std::cout << "║     C++98 Implementation         ║" << std::endl;
	// std::cout << "╚══════════════════════════════════╝" << std::endl;
	// std::cout << std::endl;

	// try
	// {
	// 	Server server;

	// 	// Run server
	// 	server.run();
		
	// 	std::cout << "\n" << std::string(40, '=') << std::endl;
	// 	std::cout << "[STATUS] Server shutdown complete" << std::endl;
	// }
	// catch (const std::runtime_error& e)
	// {
	// 	std::cerr << "\n[FATAL] Runtime error: " << e.what() << std::endl;
	// 	return 1;
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cerr << "\n[FATAL] Exception: " << e.what() << std::endl;
	// 	return 1;
	// }
	// catch (...)
	// {
	// 	std::cerr << "\n[FATAL] Unknown error occurred" << std::endl;
	// 	return 1;
	// }

// 	try {
// 		ConfigParser parser(av[1]);
// 		std::vector<ServerConfig> servers = parser.parser();
// 		for (size_t i = 0; i < servers.size(); ++i)
// 		{
// 			std::cout << "Server " << i + 1 << ":\n";
// 			std::cout << "  Server Name: " << servers[i].server_name << "\n";
// 			std::cout << "  Listen: " << servers[i].listenList.first << ":" << servers[i].listenList.second << "\n";
// 			std::cout << "  Max Body Size: " << servers[i].max_body_size << "\n";
// 			std::cout << "  Error Pages:\n";
// 			for (std::map<int, std::string>::const_iterator it = servers[i].error_pages.begin(); it != servers[i].error_pages.end(); ++it)
// 			{
// 				std::cout << "    " << it->first << " -> " << it->second << "\n";
// 			}
// 			std::cout << std::endl;
// 		}
// 	} catch (const std::exception& e)
// 	{
// 		std::cerr << "Error: " << e.what() << std::endl;
// 		return 1;
// 	}


	return 0;
}
