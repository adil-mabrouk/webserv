#ifndef REQUESTBODY_HPP
#define REQUESTBODY_HPP

#include <string>
#include <algorithm>

using std::string;

class	BoundaryParser
{
public:
	bool	isMIMEPartHeaders(string);
	bool	isTransportPadding(string);
	static bool	isLWSPChar(char);
	bool	isMultipartBody(string);
	bool	isDashBoundary(string);
	bool	isBoundary(string);
	bool	isBChars(char);
	bool	isBCharsNoSpace(char);
};

class	RequestBody
{
private:
	string	body;
public:
	void			parse();
	const string&	getBody() const;
	bool			isBoundary();
};

#endif
