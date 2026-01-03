#include "RequestLine.hpp"
#include "../Response/Response.hpp"

inline bool URIParser::isAbsPath(string str)
{
	return (str[0] == '/'
			&& isRelPath(string(str.begin() + 1, str.end())));
}

// rel_path       = [ path ] [ ";" params ] [ "?" query ]
inline bool URIParser::isRelPath(string str)
{
	string::iterator	it;
	string::iterator	it2;

	if (str.find('?', 0) != string::npos
		&& str.find(';', 0) > str.find('?', 0))
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
		return (1);
	}
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
	return (isPchar(str));
}

inline bool URIParser::isSegment(string str)
{
	return (!str.size() || isPchar(str));
}

inline bool URIParser::isParams(string str)
{
	if (!str.size())
		return (1);
	for (string::iterator it = str.begin() + 1, it_tmp;
			it != str.end();)
	{
		it_tmp = find(it, str.end(), ';');
		if (!isParam(string(it, it_tmp)))
			return (0);
		if (it_tmp != str.end())
			it = it_tmp + 1;
		else
			it = str.end();
	}
	return (1);
}

inline bool URIParser::isParam(string str)
{
	return (!str.size()
		 || (str.size() == 1 && str[0] == '/')
		 || isPchar(str));
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

inline bool	URIParser::isPchar(string str)
{
	for (string::iterator it = str.begin();
		it != str.end(); it++)
	{
		if (*it == '%' && (string(it + 1, str.end()).size() < 2
			|| !isHex(*(it + 1)) || !isHex(*(it + 2))))
			return (cout << "parse error: " << string(it, it + 2) << '\n', 0);
		else if (*it == '%')
			it += 2;
		else if (*it != ':' && *it != '@' && *it != '&'
			&& *it != '=' && *it != '+' && !isAlpha(*it)
			&& !isDigit(*it) && !isSafe(*it) && !isExtra(*it)
			&& !isNational(*it))
			return (cout << "parse error=> " << *it << '\n', 0);
	}
	return (1);
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

void	RequestLine::rootingPath(string path, string location_root, string server_root)
{
	if (!uri.find(path, 0))
	{
		string	uri_tmp;
		uri.erase(0, path.size());
		if(*server_root.rbegin() != '/' && location_root[0] != '/')
			uri_tmp = server_root + "/" + location_root;
		else
			uri_tmp = server_root + location_root;

		if(*uri_tmp.rbegin() != '/' && uri[0] != '/')
			uri = uri_tmp + "/" + uri;
			else
			uri = uri_tmp + uri;
		// uri = server_root + location_root + uri;
	}
	else
		throw 404;
}

void	RequestLine::removeDupSl()
{
	string	uri_tmp;

	for (size_t index = 0; index != uri.size(); index++)
	{
		if (!index)
			uri_tmp.assign(1, uri[0]);
		else if (uri[index] == '/' && uri_tmp[uri_tmp.size() - 1] != '/')
			uri_tmp.append(1, uri[index]);
		else if (uri[index] != '/')
			uri_tmp.append(1, uri[index]);
	}
	uri = uri_tmp;
}

void	RequestLine::removeDotSegments()
{
	string	uri_tmp;
	size_t	index;
	bool	flag;

	flag = false;
	if (uri[0] != '/')
		uri = '/' + uri, flag = true;
	while (uri.size())
	{
		if (uri[0] == '/')
			uri.erase(uri.begin());
		index = uri.find('/', 0);
		if (index == string::npos)
			index = uri.size();
		if (string(uri.begin(), uri.begin() + index) == ".")
			uri.erase(0, 1);
		else if (string(uri.begin(), uri.begin() + index) == "..")
		{
			if (!uri_tmp.size())
				throw 400;
			uri.erase(0, 2);
			index = uri_tmp.rfind('/', uri_tmp.size());
			if (index == string::npos)
			{
				if (uri_tmp.size())
					uri_tmp.clear();
				else
					throw 400;
			}
			else
				uri_tmp.erase(uri_tmp.begin() + index, uri_tmp.end());
		}
		else 
		{
			uri_tmp.append("/").append(uri.begin(), uri.begin() + index);
			uri.erase(0, index);
		}
	}
	if (flag)
		uri_tmp.erase(uri_tmp.begin());
	uri = uri_tmp;
}


void	RequestLine::parse(string str)
{
	size_t	index;
	size_t	index_2;
	size_t	query_index;

	index = str.find(' ', 0);
	if (index == string::npos)
		throw (400);
	method.assign(str.begin(), str.begin() + index);
	if (!method.size())
		throw (400);
	if (method.compare("GET")
		&& method.compare("POST")
		&& method.compare("DELETE"))
		throw (501);

	index_2 = str.find(' ', index + 1);
	if (index_2 == string::npos)
		throw (400);
	uri.assign(str.begin() + index + 1, str.begin() + index_2);
	if(!uri_parser.isAbsPath(uri))
		throw (400);
	// cout << "=> uri before encoding: " << uri << '\n';
	URIEncoding();
	// cout << "=> uri after encoding: " << uri << '\n';
	query_index = uri.find('?', 0);
	if (query_index != string::npos)
		query.assign(uri.begin() + query_index + 1, uri.end());
	if (uri.find(';', 0) < query_index)
		uri.erase(uri.begin() + uri.find(';', 0), uri.end());
	else if (uri.find(';', 0) > query_index)
		uri.erase(uri.begin() + query_index, uri.end());

	string http_version(str.begin() + index_2 + 1, str.end());
	if (http_version.compare("HTTP/1.0")
		&& http_version.compare("HTTP/1.1"))
		throw (400);
}

void	RequestLine::URIEncoding()
{
	for (string::iterator it = uri.begin(); it != uri.end(); it++)
	{
		if (*it == '%' && string(it + 1, uri.end()).size() >= 2)
		{
			if (string(it + 1, it + 3) == "00")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 0);
			else if (string(it + 1, it + 3) == "01")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 1);
			else if (string(it + 1, it + 3) == "02")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 2);
			else if (string(it + 1, it + 3) == "03")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 3);
			else if (string(it + 1, it + 3) == "04")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 4);
			else if (string(it + 1, it + 3) == "05")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 5);
			else if (string(it + 1, it + 3) == "06")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 6);
			else if (string(it + 1, it + 3) == "07")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 7);
			else if (string(it + 1, it + 3) == "08")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 8);
			else if (string(it + 1, it + 3) == "09")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 9);
			else if (string(it + 1, it + 3) == "0A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 10);
			else if (string(it + 1, it + 3) == "0B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 11);
			else if (string(it + 1, it + 3) == "0C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 12);
			else if (string(it + 1, it + 3) == "0D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 13);
			else if (string(it + 1, it + 3) == "0E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 14);
			else if (string(it + 1, it + 3) == "0F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 15);
			else if (string(it + 1, it + 3) == "10")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 16);
			else if (string(it + 1, it + 3) == "11")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 17);
			else if (string(it + 1, it + 3) == "12")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 18);
			else if (string(it + 1, it + 3) == "13")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 19);
			else if (string(it + 1, it + 3) == "14")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 20);
			else if (string(it + 1, it + 3) == "15")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 21);
			else if (string(it + 1, it + 3) == "16")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 22);
			else if (string(it + 1, it + 3) == "17")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 23);
			else if (string(it + 1, it + 3) == "18")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 24);
			else if (string(it + 1, it + 3) == "19")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 25);
			else if (string(it + 1, it + 3) == "1A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 26);
			else if (string(it + 1, it + 3) == "1B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 27);
			else if (string(it + 1, it + 3) == "1C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 28);
			else if (string(it + 1, it + 3) == "1D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 29);
			else if (string(it + 1, it + 3) == "1E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 30);
			else if (string(it + 1, it + 3) == "1F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 31);
			else if (string(it + 1, it + 3) == "20")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 32);
			else if (string(it + 1, it + 3) == "21")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 33);
			else if (string(it + 1, it + 3) == "22")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 34);
			else if (string(it + 1, it + 3) == "23")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 35);
			else if (string(it + 1, it + 3) == "24")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 36);
			else if (string(it + 1, it + 3) == "25")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 37);
			else if (string(it + 1, it + 3) == "26")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 38);
			else if (string(it + 1, it + 3) == "27")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 39);
			else if (string(it + 1, it + 3) == "28")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 40);
			else if (string(it + 1, it + 3) == "29")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 41);
			else if (string(it + 1, it + 3) == "2A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 42);
			else if (string(it + 1, it + 3) == "2B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 43);
			else if (string(it + 1, it + 3) == "2C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 44);
			else if (string(it + 1, it + 3) == "2D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 45);
			else if (string(it + 1, it + 3) == "2E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 46);
			else if (string(it + 1, it + 3) == "2F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 47);
			else if (string(it + 1, it + 3) == "30")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 48);
			else if (string(it + 1, it + 3) == "31")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 49);
			else if (string(it + 1, it + 3) == "32")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 50);
			else if (string(it + 1, it + 3) == "33")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 51);
			else if (string(it + 1, it + 3) == "34")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 52);
			else if (string(it + 1, it + 3) == "35")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 53);
			else if (string(it + 1, it + 3) == "36")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 54);
			else if (string(it + 1, it + 3) == "37")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 55);
			else if (string(it + 1, it + 3) == "38")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 56);
			else if (string(it + 1, it + 3) == "39")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 57);
			else if (string(it + 1, it + 3) == "3A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 58);
			else if (string(it + 1, it + 3) == "3B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 59);
			else if (string(it + 1, it + 3) == "3C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 60);
			else if (string(it + 1, it + 3) == "3D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 61);
			else if (string(it + 1, it + 3) == "3E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 62);
			else if (string(it + 1, it + 3) == "3F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 63);
			else if (string(it + 1, it + 3) == "40")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 64);
			else if (string(it + 1, it + 3) == "41")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 65);
			else if (string(it + 1, it + 3) == "42")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 66);
			else if (string(it + 1, it + 3) == "43")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 67);
			else if (string(it + 1, it + 3) == "44")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 68);
			else if (string(it + 1, it + 3) == "45")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 69);
			else if (string(it + 1, it + 3) == "46")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 70);
			else if (string(it + 1, it + 3) == "47")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 71);
			else if (string(it + 1, it + 3) == "48")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 72);
			else if (string(it + 1, it + 3) == "49")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 73);
			else if (string(it + 1, it + 3) == "4A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 74);
			else if (string(it + 1, it + 3) == "4B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 75);
			else if (string(it + 1, it + 3) == "4C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 76);
			else if (string(it + 1, it + 3) == "4D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 77);
			else if (string(it + 1, it + 3) == "4E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 78);
			else if (string(it + 1, it + 3) == "4F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 79);
			else if (string(it + 1, it + 3) == "50")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 80);
			else if (string(it + 1, it + 3) == "51")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 81);
			else if (string(it + 1, it + 3) == "52")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 82);
			else if (string(it + 1, it + 3) == "53")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 83);
			else if (string(it + 1, it + 3) == "54")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 84);
			else if (string(it + 1, it + 3) == "55")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 85);
			else if (string(it + 1, it + 3) == "56")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 86);
			else if (string(it + 1, it + 3) == "57")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 87);
			else if (string(it + 1, it + 3) == "58")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 88);
			else if (string(it + 1, it + 3) == "59")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 89);
			else if (string(it + 1, it + 3) == "5A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 90);
			else if (string(it + 1, it + 3) == "5B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 91);
			else if (string(it + 1, it + 3) == "5C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 92);
			else if (string(it + 1, it + 3) == "5D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 93);
			else if (string(it + 1, it + 3) == "5E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 94);
			else if (string(it + 1, it + 3) == "5F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 95);
			else if (string(it + 1, it + 3) == "60")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 96);
			else if (string(it + 1, it + 3) == "61")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 97);
			else if (string(it + 1, it + 3) == "62")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 98);
			else if (string(it + 1, it + 3) == "63")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 99);
			else if (string(it + 1, it + 3) == "64")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 100);
			else if (string(it + 1, it + 3) == "65")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 101);
			else if (string(it + 1, it + 3) == "66")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 102);
			else if (string(it + 1, it + 3) == "67")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 103);
			else if (string(it + 1, it + 3) == "68")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 104);
			else if (string(it + 1, it + 3) == "69")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 105);
			else if (string(it + 1, it + 3) == "6A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 106);
			else if (string(it + 1, it + 3) == "6B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 107);
			else if (string(it + 1, it + 3) == "6C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 108);
			else if (string(it + 1, it + 3) == "6D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 109);
			else if (string(it + 1, it + 3) == "6E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 110);
			else if (string(it + 1, it + 3) == "6F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 111);
			else if (string(it + 1, it + 3) == "70")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 112);
			else if (string(it + 1, it + 3) == "71")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 113);
			else if (string(it + 1, it + 3) == "72")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 114);
			else if (string(it + 1, it + 3) == "73")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 115);
			else if (string(it + 1, it + 3) == "74")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 116);
			else if (string(it + 1, it + 3) == "75")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 117);
			else if (string(it + 1, it + 3) == "76")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 118);
			else if (string(it + 1, it + 3) == "77")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 119);
			else if (string(it + 1, it + 3) == "78")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 120);
			else if (string(it + 1, it + 3) == "79")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 121);
			else if (string(it + 1, it + 3) == "7A")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 122);
			else if (string(it + 1, it + 3) == "7B")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 123);
			else if (string(it + 1, it + 3) == "7C")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 124);
			else if (string(it + 1, it + 3) == "7D")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 125);
			else if (string(it + 1, it + 3) == "7E")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 126);
			else if (string(it + 1, it + 3) == "7F")
				uri.replace(std::distance(uri.begin(), it), 3, 1, 127);
		}
	}
}

string	RequestLine::getMethod() const
{
	return (method);
}

string	RequestLine::getURI() const
{
	return (uri);
}

string	RequestLine::getQuery() const
{
	return (query);
}
