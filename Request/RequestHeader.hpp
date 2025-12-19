#ifndef REQUESTHEADER_HPP
#define REQUESTHEADER_HPP

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

using std::string;
using std::map;

// look for the role of every header,
// does it affect the program behavior
// if a header is missing or has different value (ex: the client is User-Agent)
class	RequestHeader
{
private:
	map<const string, const string>	header_data;
public:
	void	parse(string);

	bool	isFieldName(string) const;
	bool	isFieldBody(string) const;

	const map<const string, const string>&	getHeaderData();
};

#endif
