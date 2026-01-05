#ifndef REQUESTHEADER_HPP
#define REQUESTHEADER_HPP

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

using std::string;
using std::map;

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
