#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>
#include <map>

struct CGIConfig {
	std::string									extension;
	std::string									path;
};

struct RedirectConfig {
	size_t										status_code;
	std::string									url;
};

struct LocationConfig {
	std::string									path;
	std::string									root;
	std::vector<std::string>					index;
	bool										autoindex;
	std::vector<std::string>					methods;
	RedirectConfig								redirect;
	bool										redirectExist;
	std::string									upload_store;
	bool										allow_upload;
	std::vector<CGIConfig>						cgi;
	bool										hasCGI;
	LocationConfig() : autoindex(false), redirectExist(false), allow_upload(false), hasCGI(false) {}
};

struct ServerConfig {
	std::vector<std::pair<std::string, int> >	listenList;
	std::map<int, std::string>					error_pages;
	std::map<std::string, LocationConfig>		locations;
	std::string									root; // added for server root directive
	long long									max_body_size;
	bool										maxBodySizeExist;
	ServerConfig() : max_body_size(0), maxBodySizeExist(false) {}
};

class ConfigParser {
	public:
		ConfigParser(const std::string&);
		std::vector<ServerConfig>					parser();
		std::vector<std::pair<std::string, int> >	allListenDirectives;
	private:
		void									defaultServerParams(ServerConfig& serverConfig);
		void									defaultLocationParams(LocationConfig& locConfig);
		std::string 							_content;
		std::vector<std::string>				tokenize(const std::string& text);
		ServerConfig							parseServerBlock(const std::vector<std::string>& , size_t&);
		LocationConfig							parseLocationBlock(const std::vector<std::string>& , size_t&);
		void									parseListenDirective(const std::vector<std::string>&, size_t&, ServerConfig&);
		void									parseClientMaxBody(const std::vector<std::string>&, size_t&, ServerConfig&);
		void									parseErrorPage(const std::vector<std::string>&, size_t&, ServerConfig&);
};
