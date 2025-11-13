#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

struct ServerConfig {};

struct LocationConfig {};

class ConfigParser {
	public:
		ConfigParser(const std::string& path);
		std::vector<ServerConfig>			parser();
	private:
		std::string 						_content;
		std::vector<std::string>			tokenize(const std::string& text);
		ServerConfig						parseServerBlock(const std::vector<std::string>& tokens, size_t& index);
		LocationConfig						parseLocationBlock(const std::vector<std::string>& tokens, size_t& index);
};

struct ServerConfig {
	std::string 							server_name;
	int 									port;
	std::string								host;
	std::map<int, std::string>				error_pages;
	std::map<std::string, LocationConfig>	locations;
	size_t 									max_body_size;
};

struct LocationConfig {
	std::string								path;
	std::string								root;
	std::string								index;
	bool									autoindex;
	std::vector<std::string>				methods;
	std::string								redirect;
	std::string								upload_store;
	bool									allow_upload;
	std::string								cgi_extension;
	std::string								cgi_path;
};
