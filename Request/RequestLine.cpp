#include "RequestLine.hpp"

bool URIParser::isURI(string str)
{
	string::iterator	it;

	it = find(str.begin(), str.end(), '#');
	return ((isAbsoluteURI(string(str.begin(), it))
				|| isRelativeURI(string(str.begin(), it)))
				&& (it == str.end()
					|| isFragment(string(it + 1, str.end()))));
}

inline bool URIParser::isAbsoluteURI(string str)
{
	string::iterator	it;

	it = find(str.begin(), str.end(), ':');
	if (it == str.end() || !isScheme(string(str.begin(), it)))
		return (0);
	it++;
	return (it == str.end()
			|| isUchar(string(it, str.end()))
			|| isReserved(string(it, str.end())));
}

inline bool URIParser::isRelativeURI(string str)
{
	return (isNetPath(str) || isAbsPath(str)
			|| isRelPath(str));
}

inline bool URIParser::isNetPath(string str)
{
	if (str.size() < 2 || str[0] != '/'
			|| str[1] != '/')
		return (0);
	string::iterator	it;

	it = find(str.begin(), str.end(), '/');
	return (isNetLoc(string(str.begin() + 2, it))
			&& (it == str.end()
				|| isAbsPath(string(it, str.end()))));
}

inline bool URIParser::isAbsPath(string str)
{
	return (str.size() && str[0] == '/'
			&& isRelPath(string(str.begin() + 1, str.end())));
}

inline bool URIParser::isRelPath(string str)
{
	string::iterator	it;
	string::iterator	it2;

	it = find(str.begin(), str.end(), ';');
	if (it == str.end())
	{
		it = find(str.begin(), str.end(), '?');
		if (it == str.end())
		{
			if (str.size())
				if (!isPath(str))
					return (0);
		}
		else
		{
			if (distance(str.begin(), it))
				if (!isPath(string(str.begin(), it)))
					return (0);
			it++;
			if (!isQuery(string(it, str.end())))
				return (0);
		}
	}
	else
	{
		if (distance(str.begin(), it))
			if (!isPath(string(str.begin(), it)))
				return (0);
		it++;
		it2 = find(it, str.end(), '?');
		if (!isParams(string(it, it2)))
			return (0);
		if (it2 != str.end())
		{
			it2++;
			if (!isQuery(string(it2, str.end())))
				return (0);
		}
	}
	return (1);
}

inline bool URIParser::isPath(string str)
{
	string::iterator it = find(str.begin(), str.end(), '/');

	if (!isFsegment(string(str.begin(), it)))
		return (0);
	for (string::iterator it_tmp; it != str.end();)
	{
		it++;
		it_tmp = find(it, str.end(), '/');
		if (!isSegment(string(it, it_tmp)))
			return (0);
		it = it_tmp;
	}
	return (1);
}

inline bool URIParser::isFsegment(string str)
{
	if (!str.size())
		return (0);
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isPchar(*it))
			return (0);
	return (1);
}

inline bool URIParser::isSegment(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isPchar(*it))
			return (0);
	return (1);
}

inline bool URIParser::isParams(string str)
{
	if (!str.size() || str[0] != ';')
		return (0);
	for (string::iterator it = str.begin() + 1, it_tmp;
			it != str.end();)
	{
		it_tmp = find(it, str.end(), ';');
		if (!isParam(string(it, it_tmp)))
			return (0);
		if (*it_tmp == ';')
			it = it_tmp + 1;
		else
			it = str.end();
	}
	return (1);
}

inline bool URIParser::isParam(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isPchar(*it) && *it != '/')
			return (0);
	return (1);
}

inline bool URIParser::isScheme(string str)
{
	if (!str.size())
		return (0);
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isAlpha(*it) && !isDigit(*it)
			&& *it != '+' && *it != '-' && *it != '.')
			return (0);
	return (1);
}

inline bool URIParser::isNetLoc(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isPchar(*it)
			&& *it != ';' && *it != '?')
			return (0);
	return (1);
}

inline bool URIParser::isQuery(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isUchar(string(1, *it))
				&& !isReserved(string(1, *it)))
			return (0);
	return (1);
}

inline bool URIParser::isFragment(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (!isUchar(string(1, *it))
				&& !isReserved(string(1, *it)))
			return (0);
	return (1);
}

inline bool	URIParser::isPchar(char c)
{
	return (isUchar(string(1, c)) || c == ':'
		|| c == '@' || c == '&'
		|| c == '=' || c == '+');
}

inline bool	URIParser::isUchar(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
	if (!isUnreserved(*it) && !isEscape(string(1, *it)))
		return (0);
	return (1);
}

inline bool	URIParser::isUnreserved(char c)
{
	return (isAlpha(c) || isDigit(c)
		 || isSafe(c) || isExtra(c)
		 || isNational(c));
}

inline bool URIParser::isAlpha(char c)
{
	return ((c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z'));
}

inline bool URIParser::isDigit(char c)
{
	return (c >= '0' && c <= '9');
}

inline bool URIParser::isHex(char c)
{
	return ((c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f')
		|| isDigit(c));
}

inline bool URIParser::isEscape(string str)
{
	return (str.size() >= 3 && str[0] == '%'
		&& isHex(str[1]) && isHex(str[2]));
}

inline bool URIParser::isReserved(string str)
{
	for (string::iterator it = str.begin();
			it != str.end(); it++)
		if (*it != ';' && *it != '/' && *it != '?'
				&& *it != ':' && *it != '@' && *it != '&'
				&& *it != '=' && *it != '+')
			return (0);
	return (1);
}

inline bool URIParser::isExtra(char c)
{
	return (c == '!' || c == '*' || c == '\''
		|| c == '(' || c == ')' || c == ',');
}

inline bool URIParser::isSafe(char c)
{
	return (c == '$' || c == '-'
		|| c == '_' || c == '.');
}

inline bool URIParser::isUnsafe(char c)
{
	return (((c >= 0 && c <= 31) || c == 127)
		|| c == ' ' || c == '"' || c == '#'
		|| c == '%' || c == '<' || c == '>');
}

inline bool URIParser::isNational(char c)
{
	return (!isAlpha(c) && !isDigit(c)
		&& !isReserved(string(1, c)) && !isExtra(c)
		&& !isSafe(c) && !isUnsafe(c));
}

// don't forget /r/n
void	RequestLine::parse(string str)
{
	string::iterator	it, it_tmp;

	it = find(str.begin(), str.end(), ' ');
	if (it == str.end())
		throw std::runtime_error("method parsing error");
	method.assign(str.begin(), it);
	if (method.compare("GET")
		&& method.compare("POST")
		&& method.compare("DELETE"))
		throw std::runtime_error("method parsing error");
	// std::cout << method << ' ';

	it_tmp = it + 1;
	it = find(it_tmp, str.end(), ' ');
	if (it == str.end())
		throw std::runtime_error("uri parsing error");
	uri.assign(it_tmp, it);
	if(!uri_parser.isURI(uri))
		throw std::runtime_error("uri parsing error");
	// std::cout << uri << '\n';

	it_tmp = it + 1;
	it = find(it_tmp, str.end(), ' ');
	if (it != str.end())
		throw std::runtime_error("http version parsing error");
	string http_version(it_tmp, it);
	std::cout << "|" << http_version << "|\n";
	if (http_version.compare("HTTP/1.1"))
		throw std::runtime_error("http version parsing error");
}

string	RequestLine::getMethod() const
{
	return (method);
}

string	RequestLine::getURI() const
{
	return (uri);
}
