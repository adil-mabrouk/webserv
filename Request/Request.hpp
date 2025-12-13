#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestLine.hpp"
#include "RequestHeader.hpp"
#include "RequestBody.hpp"
#include <iostream>
#include <algorithm>
#include <string>

using std::string;
using std::find;
using std::cout;
using std::cerr;

class Request
{
public:
	RequestLine		request_line;
	RequestHeader	request_header;
	// RequestBody		request_body;
	string			body;
// separate the response object from the request

	void requestParsing(string);
	string requestExec();
};

#endif
