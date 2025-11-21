#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <map>
// #include <stdexcept>
// #include <cctype>

struct LocationConfig {
	std::string									path;
	std::string									root;
	std::vector<std::string>					index;
	bool										autoindex;
	std::vector<std::string>					methods;
	std::string									redirect;
	std::string									upload_store;
	bool										allow_upload;
	std::string									cgi_extension;
	std::string									cgi_path;
};

struct ServerConfig {
	std::string 								server_name;
	std::vector<std::pair<std::string, int> >	listenList;
	std::map<int, std::string>					error_pages;
	std::map<std::string, LocationConfig>		locations;
	std::string									root; // added for server root directive
	size_t										max_body_size;
};

class ConfigParser {
	public:
		ConfigParser(const std::string& path);
		std::vector<ServerConfig>				parser();
	private:
		std::string 							_content;
		std::vector<std::string>				tokenize(const std::string& text);
		ServerConfig							parseServerBlock(const std::vector<std::string>& tokens, size_t& index);
		LocationConfig							parseLocationBlock(const std::vector<std::string>& tokens, size_t& index);
		void									parseListenDirective(const std::vector<std::string> &tokens, size_t &index, ServerConfig &servConf);
		void									parseServerNameDirective(const std::vector<std::string> &tokens, size_t &index, ServerConfig &servConf);
		void									parseClientMaxBody(const std::vector<std::string> &tokens, size_t &index, ServerConfig &servConf);
		void									parseErrorPage(const std::vector<std::string> &tokens, size_t &index, ServerConfig &servConf);
};
