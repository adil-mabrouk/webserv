#include "config.hpp"

ConfigParser::ConfigParser(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + path);
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	_content = ss.str();
	file.close();
}

std::vector<ServerConfig>	ConfigParser::parser() {
	std::vector<std::string>	tokens = tokenize(_content);
	std::vector<ServerConfig>	servers;
	size_t						index = 0;
	while (index < tokens.size()) {
		if (tokens[index] == "server") {
			ServerConfig server = parseServerBlock(tokens, index);
			servers.push_back(server);
		} else {
			++index;
		}
	}
	return servers;
}

std::vector<std::string>	ConfigParser::tokenize(const std::string& text) {
	std::vector<std::string>	tokens;
	std::string					token;
	for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
		char c = *it;
		if (std::isspace(static_cast<unsigned char>(c))) {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
		}
		else if (c == '{' || c == '}' || c == ';') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(std::string(1, c));
		}
		else
			token += c;
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

ServerConfig	ConfigParser::parseServerBlock(const std::vector<std::string> &tokens, size_t &index) {
	++index; // Skip "server"
	if (tokens[index] != "{") {
		throw std::runtime_error("Expected '{' after server");
	}
	++index; // Skip "{"

	ServerConfig serverConfig;

	while (index < tokens.size() && tokens[index] != "}") {
		if (tokens[index] == "location") {
			LocationConfig location = parseLocationBlock(tokens, index);
			serverConfig.locations[location.path] = location;
		} 
		else if (tokens[index] == "listen") {
			++index; // Skip "listen"
			if (index >= tokens.size()) {
				throw std::runtime_error("Expected port number after listen");
			}
			std::istringstream ss(tokens[index]);
			int	port;
			ss >> port;
			if (ss.fail()) {
				throw std::runtime_error("Invalid port number in listen directive");
			}
			serverConfig.port = port;
			++index; // Skip port number
			if (index >= tokens.size() || tokens[index] != ";") {
				throw std::runtime_error("Expected ';' after listen directive");
			}
			++index; // Skip ";"
		} 
		else if (tokens[index] == "server_name") {
			++index; // Skip "server_name"
			if (index >= tokens.size()) {
				throw std::runtime_error("Expected server name after server_name");
			}
			serverConfig.server_name = tokens[index];
			++index; // Skip server name
			if (index >= tokens.size() || tokens[index] != ";") {
				throw std::runtime_error("Expected ';' after server_name directive");
			}
			++index; // Skip ";"
		} 
		else if (tokens[index] == "client_max_body_size") {
			++index; // Skip "client_max_body_size"
			if (index >= tokens.size()) {
				throw std::runtime_error("Expected size after client_max_body_size");
			}
			std::istringstream sizeStream(tokens[index]);
			size_t	size;
			sizeStream >> size;
			if (sizeStream.fail()) {
				throw std::runtime_error("Invalid size for client_max_body_size");
			}
			serverConfig.max_body_size = size;
			++index; // Skip size
			if (index >= tokens.size() || tokens[index] != ";") {
				throw std::runtime_error("Expected ';' after client_max_body_size directive");
			}
			++index; // Skip ";"
		}
		else if (tokens[index] == "error_page") {
			++index; // Skip "error_page"
			if (index + 1 >= tokens.size()) {
				throw std::runtime_error("Expected error code and page after error_page");
			}
			int error_code;
			std::istringstream codeStream(tokens[index]);
			codeStream >> error_code;
			if (codeStream.fail()) {
				throw std::runtime_error("Invalid error code in error_page directive");
			}
			++index; // Skip error code
			std::string error_page = tokens[index];
			serverConfig.error_pages[error_code] = error_page;
			++index; // Skip error page
			if (index >= tokens.size() || tokens[index] != ";") {
				throw std::runtime_error("Expected ';' after error_page directive");
			}
			++index; // Skip ";"
		}
		else {
			++index; // Skip unknown directive
		}
	}

	if (index >= tokens.size() || tokens[index] != "}") {
		throw std::runtime_error("Expected '}' at the end of server block");
	}
	++index; // Skip "}"

	return serverConfig;
}