#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include <string>
#include <algorithm>
#include <stdexcept>
#include <iostream>

using std::distance;
using std::string;

class URIParser
{
public:
	bool	isURI(string);

	inline bool isAbsoluteURI(string);
	inline bool isRelativeURI(string);

	inline bool isNetPath(string);
	inline bool isAbsPath(string);
	inline bool isRelPath(string);

	inline bool isPath(string);
	inline bool isFsegment(string);
	inline bool isSegment(string);

	inline bool isParams(string);
	inline bool isParam(string);

	inline bool isScheme(string);
	inline bool isNetLoc(string);
	inline bool isQuery(string);
	inline bool isFragment(string);

	inline bool isPchar(char);
	inline bool isUchar(string);
	inline bool isUnreserved(char);

	inline bool	isHex(char);
	inline bool	isAlpha(char);
	inline bool	isDigit(char);

	inline bool isEscape(string);
	inline bool isReserved(string);
	inline bool isExtra(char);
	inline bool isSafe(char);
	inline bool isUnsafe(char);
	inline bool isNational(char);

};

class	RequestLine
{
private:
	string		method;
	string		uri;
	URIParser	uri_parser;
public:
	void	parse(string);
	void	rootingPath(string, string, string);
	void	removeDupSl();
	void	removeDotSegments();
	string	getMethod() const;
	string	getURI() const;
};

#endif
