#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestLine.hpp"
#include "RequestHeader.hpp"
#include "../Response/Response.hpp"
#include <iostream>
#include <algorithm>
#include <string>

using std::string;
using std::find;
using std::cout;
using std::cerr;

class Request
{
private:
	RequestLine		request_line;
	RequestHeader	request_header;
	Response		response;
public:
	void request_parsing(string);
	string request_exec();
};

#endif
