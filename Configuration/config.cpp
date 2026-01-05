#include "config.hpp"

ConfigParser::ConfigParser(const std::string& path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open config file: " + path);
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	_content = ss.str();
	file.close();
}

void	ConfigParser::defaultServerParams(ServerConfig& serverConfig)
{
	serverConfig.root = "/home/amabrouk/Desktop/webserv";
}

ServerConfig::ServerConfig() : max_body_size(std::numeric_limits<unsigned long>::max()) {}

LocationConfig::LocationConfig() : autoindex(false), redirectExist(false), allow_upload(false) {}

std::vector<ServerConfig>	ConfigParser::parser()
{
	std::vector<std::string>	tokens = tokenize(_content);
	std::vector<ServerConfig>	servers;
	size_t						index = 0;
	while (index < tokens.size())
	{
		if (tokens[index] == "server")
		{
			ServerConfig server = parseServerBlock(tokens, index);
			if (server.listenList.size() == 0)
				throw std::runtime_error("No listen directive provided");
			servers.push_back(server);
		}
		else
		{
			++index;
		}
	}
	return servers;
}

std::vector<std::string>	ConfigParser::tokenize(const std::string& text)
{
	std::vector<std::string>	tokens;
	std::string					token;
	for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
	{
		char c = *it;
		if (std::isspace(static_cast<unsigned char>(c)))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else if (c == '{' || c == '}' || c == ';')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(std::string(1, c));
		}
		else
			token += c;
	}
	if (!token.empty())
	{
		tokens.push_back(token);
	}
	return tokens;
}

static bool isValidMethod(const std::string& method)
{
	return method == "GET" || method == "POST" || method == "DELETE";
}

static bool	isValidCgiExtension(const std::string &ext, const std::string &path)
{
	return ((ext == ".py" && path == "/usr/bin/python3") || (ext == ".sh" && path == "/usr/bin/bash"));
}

LocationConfig	ConfigParser::parseLocationBlock(const std::vector<std::string>& tokens, size_t& index)
{
	++index; 
	if (index >= tokens.size())
		throw std::runtime_error("Expected path after location");
	LocationConfig locConfig;
	defaultLocationParams(locConfig);

	locConfig.path = tokens[index++];

	if (tokens[index] != "{")
		throw std::runtime_error("Expected '{' after location path");
	++index; 

	while (index < tokens.size() && tokens[index] != "}")
	{
		if (tokens[index] == "root")
		{
			++index; 
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after root directive");
			locConfig.root = tokens[index++];
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after root directive");
			++index; 
		}
		else if (tokens[index] == "index")
		{
			locConfig.index.clear();
			++index; 
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after index directive");
			while (index < tokens.size() && tokens[index] != ";")
				locConfig.index.push_back(tokens[index++]);
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after index directive");
			++index; 
		}
		else if (tokens[index] == "autoindex")
		{
			++index; 
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after autoindex directive");
			std::string value = tokens[index++];
			if (value == "on")
				locConfig.autoindex = true;
			else if (value == "off")
				locConfig.autoindex = false;
			else
				throw std::runtime_error("Invalid value for autoindex directive");
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after autoindex directive");
			++index; 
		}
		else if (tokens[index] == "methods")
		{
			++index;
			while (index < tokens.size() && tokens[index] != ";")
			{
				if (isValidMethod(tokens[index]))
					locConfig.methods.push_back(tokens[index++]);
				else
					throw std::runtime_error("Invalid HTTP method in methods directive: " + tokens[index]);
			}
			if (index >= tokens.size() || tokens[index] != ";")
				throw std::runtime_error("Expected ';' after methods directive");
			++index;
		}
		else if (tokens[index] == "return")
		{
			++index;
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after return directive");
			const std::string& token = tokens[index++];
			std::istringstream	ss(token);
			ss >> locConfig.redirect.status_code;
			if (ss.fail() || !ss.eof())
				throw std::runtime_error("Invalid status code in error_page directive");
			locConfig.redirect.url = tokens[index++];
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after return directive");
			++index;
			locConfig.redirectExist = true;
		}
		else if (tokens[index] == "upload_store")
		{
			++index;
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after upload_store directive");
			locConfig.upload_store = tokens[index++];
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after upload_store directive");
			++index;
		}
		else if (tokens[index] == "allow_upload")
		{
			++index;
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after allow_upload directive");
			std::string value = tokens[index++];
			if (value == "on")
				locConfig.allow_upload = true;
			else if (value == "off")
				locConfig.allow_upload = false;
			else
				throw std::runtime_error("Invalid value for allow_upload directive");
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after allow_upload directive");
			++index;
		}
		else if (tokens[index] == "cgi_extension")
		{
			++index;
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after cgi_extension directive");
			std::string extension = tokens[index++];
			std::string path = tokens[index++];
			if (!isValidCgiExtension(extension, path))
				throw std::runtime_error("Invalid cgi_extension");
			CGIConfig cgiConfig;
			cgiConfig.extension = extension;
			cgiConfig.path = path;
			locConfig.cgi.push_back(cgiConfig);
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after cgi_extension directive");
			++index;
		}
		else
			throw std::runtime_error("Unknown directive inside location block: " + tokens[index]);
	}

	if (index >= tokens.size() || tokens[index] != "}")
		throw std::runtime_error("Expected '}' at the end of location block");
	++index;

	return locConfig;
}

static bool	isValidIpAddress(const std::string& ip)
{
	int count = 0;
	for (size_t i = 0; i < ip.size(); ++i)
	{
		if (ip[i] == '.')
			count++;
	}
	if (count != 3)
		return false;
	std::stringstream	ss(ip);
	std::string			byte;
	int					partCount = 0;
	while (std::getline(ss, byte, '.'))
	{
		if (byte.empty() || byte.size() > 3)
			return false;
		for (size_t i = 0; i < byte.size(); i++)
			if (!isdigit(byte[i])) return false;
		int value;
		std::stringstream	sv(byte);
		sv >> value;
		if (sv.fail() || value < 0 || value > 255)
			return false;
		partCount++;
	}
	return partCount == 4;
}

void	ConfigParser::parseListenDirective(const std::vector<std::string>& tokens, size_t& index, ServerConfig& serverConfig)
{
	++index;
	if (index >= tokens.size())
		throw std::runtime_error("Expected value after listen directive");
	std::string listenValue = tokens[index++];
	if (tokens[index] != ";")
		throw std::runtime_error("Expected ';' after listen directive");

	++index;
	std::pair<std::string, int> listenPair;
	listenPair.first = "0.0.0.0";
	size_t colonPos = listenValue.find(':');
	if (colonPos != std::string::npos)
	{
		listenPair.first = listenValue.substr(0, colonPos);  
		if (!isValidIpAddress(listenPair.first))
			throw std::runtime_error("Invalid ip address in listen directive");
		std::istringstream	ss(listenValue.substr(colonPos + 1));
		ss >> listenPair.second;  
		if (ss.fail() || !ss.eof() || listenPair.second <= 0 || listenPair.second > 65535)
			throw std::runtime_error("Invalid port in listen directive");
	}
	else
	{
		std::istringstream	ss(listenValue);
		ss >> listenPair.second;
		if (ss.fail() || listenPair.second <= 0 || listenPair.second > 65535)
			throw std::runtime_error("Invalid port number in listen directive");
	}
	for (std::vector<std::pair<std::string, int> >::iterator it = allListenDirectives.begin();
		it != allListenDirectives.end(); it++)
	{
		if ((it->second == listenPair.second)
			&& (it->first == listenPair.first || it->first == "0.0.0.0"))
			throw std::runtime_error("Duplicate listen directive");
	}
	serverConfig.listenList.push_back(listenPair);
	allListenDirectives.push_back(listenPair);
}

static size_t	convertToBytes(std::string &unit)
{
	size_t multiplier = 1;
    
    if (!unit.empty())
    {
        if (unit.size() > 1)
            throw std::runtime_error("Invalid unit in client_max_body_size");
        
        if (unit == "K" || unit == "k")
            multiplier = 1024;
        else if (unit == "M" || unit == "m")
            multiplier = 1024 * 1024;
        else if (unit == "G" || unit == "g")
            multiplier = 1024 * 1024 * 1024;
        else
            throw std::runtime_error("Invalid unit in client_max_body_size");
    }
	return multiplier;
}

void ConfigParser::parseClientMaxBody(const std::vector<std::string>& tokens, size_t& index, ServerConfig& serverConfig)
{
	++index;  
	if (index >= tokens.size())
		throw std::runtime_error("Expected value after client_max_body_size directive");
	std::string value = tokens[index++];
	 
	
	size_t numEnd = 0;
	while (numEnd < value.size() && std::isdigit(value[numEnd]))
		numEnd++;
	std::string numberPart = value.substr(0, numEnd);
	std::string unitPart = value.substr(numEnd);
	if (numberPart.empty())
		throw std::runtime_error("Invalid client_max_body_size: no number found");
	std::istringstream ss(numberPart);
	size_t maxBodySize;
	ss >> maxBodySize;
	if (ss.fail())
		throw std::runtime_error("Invalid number in client_max_body_size");
	if (maxBodySize == 0)
		throw std::runtime_error("client_max_body_size cannot be 0");
	 
	size_t	toConvert = convertToBytes(unitPart);
	if (maxBodySize > std::numeric_limits<unsigned long>::max() / toConvert)  
		throw std::runtime_error("client_max_body_size value too large");
	serverConfig.max_body_size = maxBodySize * toConvert;
	if (index >= tokens.size() || tokens[index] != ";")
		throw std::runtime_error("Expected ';' after client_max_body_size directive");
	++index;
	 
}

void	ConfigParser::parseErrorPage(const std::vector<std::string>& tokens, size_t& index, ServerConfig& serverConfig)
{
	++index;  
	if (index >= tokens.size())
		throw std::runtime_error("Expected status code and path after error_page directive");
	std::vector<int>	statusCodes;
	std::string			path;
	size_t saveIndex = index;
	while (index < tokens.size() && tokens[index] != ";")
		index++;
	path = tokens[--index];
	while (saveIndex < tokens.size())
	{
		const std::string& token = tokens[saveIndex];
		if (tokens[saveIndex + 1] == ";")
		{
			saveIndex++;
			break;
		}
		int code;
		std::istringstream	ss(token);
		ss >> code;
		if (ss.fail() || !ss.eof())
		{
			throw std::runtime_error("Invalid error_page directive");
		}
		statusCodes.push_back(code);
		saveIndex++;
	}
	if (statusCodes.empty())
		throw std::runtime_error("No status codes provided for error_page");
	if (path.empty())
		throw std::runtime_error("Missing path in error_page directive");
	if (tokens[saveIndex] != ";")
		throw std::runtime_error("Expected ';' after error_page directive");
	index = ++saveIndex;
	for (size_t i = 0; i < statusCodes.size(); i++)
		serverConfig.error_pages[statusCodes[i]] = path;
}

void	ConfigParser::defaultLocationParams(LocationConfig& locConfig)
{
	locConfig.index.push_back("index.html");
	locConfig.autoindex = false;
	locConfig.redirect.status_code = 0;  
	 
	 
	 
}

ServerConfig	ConfigParser::parseServerBlock(const std::vector<std::string> &tokens, size_t &index)
{
	++index;  
	if (tokens[index] != "{")
	{
		throw std::runtime_error("Expected '{' after server");
	}
	++index;  

	ServerConfig serverConfig;
	defaultServerParams(serverConfig);
	while (index < tokens.size() && tokens[index] != "}")
	{
		if (tokens[index] == "location")
		{
			LocationConfig loc;
			loc = parseLocationBlock(tokens, index);
			serverConfig.locations[loc.path] = loc;
			if (serverConfig.locations[loc.path].root.empty())
				serverConfig.locations[loc.path].root = serverConfig.root;  
		}
		else if (tokens[index] == "listen")
			parseListenDirective(tokens, index, serverConfig);
		else if (tokens[index] == "client_max_body_size")
			parseClientMaxBody(tokens, index, serverConfig);
		else if (tokens[index] == "error_page")
			parseErrorPage(tokens, index, serverConfig);
		else if (tokens[index] == "root")
		{
			++index;  
			if (index >= tokens.size())
				throw std::runtime_error("Expected value after root directive");
			serverConfig.root = tokens[index++];
			if (tokens[index] != ";")
				throw std::runtime_error("Expected ';' after root directive");
			++index;  
		}
		else
			throw std::runtime_error("Unknown directive inside server block: " + tokens[index]);
	}
	if (index >= tokens.size() || tokens[index] != "}")
	{
		throw std::runtime_error("Expected '}' at the end of server block");
	}
	++index;  
	if (index < tokens.size() && tokens[index] != "server")
	throw std::runtime_error("Expected 'server' or end of configuration after server block");
	
	 
	return serverConfig;
}
