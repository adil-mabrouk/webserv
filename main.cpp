#include "Server/Server.hpp"
#include "Configuration/config.hpp"

int main(int ac, char** av)
{
	// if (ac != 2) {
	// 	std::cerr << "Usage: ./webserv config_file\n";
	// 	return 1;
	// }

	// try {
	// 	ConfigParser				parser(av[1]);
	// 	std::vector<ServerConfig>	configs = parser.parser();
	// 	if (configs.empty()) {
	// 		std::cerr << "No server configuration found\n";
	// 		return 1;
	// 	}
	// 	std::cout << "Loaded " << configs.size() << " server block(s)\n";
	// 	Server	server(configs);
	// 	server.run();
	// }
	// catch (const std::exception &e) {
	// 	std::cerr << "Error: " << e.what() << "\n";
	// 	return 1;
	// }

	(void)ac;
	std::cout << "╔══════════════════════════════════╗" << std::endl;
	std::cout << "║     Webserv HTTP Server          ║" << std::endl;
	std::cout << "║     C++98 Implementation         ║" << std::endl;
	std::cout << "╚══════════════════════════════════╝" << std::endl;
	std::cout << std::endl;

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

	try {
		ConfigParser parser(av[1]);
		std::vector<ServerConfig> servers = parser.parser();
		for (size_t i = 0; i < servers.size(); ++i)
		{
			std::cout << "Server " << i + 1 << ":\n";
			if (!servers[i].listenList.empty()) {
				for (size_t j = 0; j < servers[i].listenList.size(); ++j) {
					std::cout << "  Listen: " << servers[i].listenList[j].first << ":" << servers[i].listenList[j].second << "\n";
				}
			} else {
				std::cout << "  Listen: (none)\n";
			}
			std::cout << "  Max Body Size: " << servers[i].max_body_size << "\n";
			std::cout << "  Error Pages:\n";
			for (std::map<int, std::string>::const_iterator it = servers[i].error_pages.begin(); it != servers[i].error_pages.end(); ++it)
			{
				std::cout << "    " << it->first << " -> " << it->second << "\n";
			}
			std::cout << std::endl;
			for (std::map<std::string, LocationConfig>::const_iterator it = servers[i].locations.begin(); it != servers[i].locations.end(); ++it)
			{
				std::cout << "  Location: " << it->first << "\n";
				std::cout << "    Root: " << it->second.root << "\n";
				std::cout << "    Index Files: ";
				for (size_t k = 0; k < it->second.index.size(); ++k)
				{
					std::cout << it->second.index[k];
					if (k < it->second.index.size() - 1)
						std::cout << ", ";
				}
				std::cout << "\n";
				std::cout << "    Autoindex: " << (it->second.autoindex ? "on" : "off") << "\n";
				std::cout << "    Methods: ";
				for (size_t k = 0; k < it->second.methods.size(); ++k)
				{
					std::cout << it->second.methods[k];
					if (k < it->second.methods.size() - 1)
						std::cout << ", ";
				}
				std::cout << "\n";
				std::cout << "    Upload Store: " << it->second.upload_store << "\n";
				std::cout << "    Redirect: " << it->second.redirect.status_code << " -> " << it->second.redirect.url << "\n";
			}
			// Print CGI configuration
			for (std::map<std::string, LocationConfig>::const_iterator it = servers[i].locations.begin(); it != servers[i].locations.end(); ++it)
			{
				if (!it->second.cgi.extension.empty() && !it->second.cgi.path.empty())
				{
					std::cout << "    CGI Extension: " << it->second.cgi.extension << "\n";
					std::cout << "    CGI Path: " << it->second.cgi.path << "\n";
				}
			}
			std::cout << std::endl;
		}
	} catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}


	return 0;
}
