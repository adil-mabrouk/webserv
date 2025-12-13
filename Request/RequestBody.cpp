#include "RequestBody.hpp"

// bool	BoundaryParser

bool	BoundaryParser::isMIMEPartHeaders(string str)
{

}

bool	BoundaryParser::isTransportPadding(string str)
{
	for (string::iterator it = str.begin();
	  it != str.end(); it++)
		if (!isLWSPChar(*it))
			return (0);
	return (1);
}

bool	BoundaryParser::isLWSPChar(char c)
{
	return (c == ' ' || c == '\t');
}

bool	BoundaryParser::isMultipartBody(string str)
{
	string::iterator	it;
	size_t				crlf_index;

	it = std::find_if(str.begin(), str.end(), isLWSPChar);
	if (it != str.end())
	{
		if (!isDashBoundary(string(str.begin(), it)))
			return (0);
		crlf_index = str.find("\r\n", std::distance(str.begin(), it));
		if (crlf_index == string::npos)
			return (0);
		else
	}
	else
	{
		crlf_index = str.find("\r\n", 0);
		if (crlf_index == string::npos ||
			!isDashBoundary(string(str.begin(),
						  str.begin() + crlf_index)))
			return (0);
	}
}

bool	BoundaryParser::isDashBoundary(string str)
{
	if (str.size() < 3)
		return (0);
	return (!string(str.begin(), str.begin() + 2).compare("--")
	&& isBoundary(string(str.begin() + 2, str.end())));
}

bool	BoundaryParser::isBoundary(string str)
{
	if (!str.size() || str.size() > 70)
		return (0);
	for (size_t index = 0; index < str.size() - 1; index++)
		if (!isBChars(str[index]))
			return (0);
	return (isBCharsNoSpace(str[str.size() - 1]));
}

bool	BoundaryParser::isBChars(char c)
{
	return (isBCharsNoSpace(c) || c == ' ');
}

bool	BoundaryParser::isBCharsNoSpace(char c)
{
	return ((c >= '0' && c <= '9') ||
	((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) ||
	c == '\'' || c == '(' || c == ')' || c == '+' ||
	c == '_' || c == ',' || c == '-' || c == '.' ||
	c == '/' || c == ':' || c == '=' || c == '?');
}

void	RequestBody::parse()
{

}

const string&	RequestBody::getBody() const
{
	return (body);
}
