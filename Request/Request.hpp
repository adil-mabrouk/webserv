#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestLine.hpp"
#include "RequestHeader.hpp"
#include <algorithm>
#include <iostream>
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

	// string requestExec();
};

#endif
