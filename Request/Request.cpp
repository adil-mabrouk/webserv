#include "Request.hpp"
#include "../Response/Response.hpp"

// modfifie this parsing so it expect every part as separated to parse
void	Request::requestParsing(string request)
{
	size_t	crlf_index;
	size_t	index_tmp;
	size_t	index_dcrlf;

	crlf_index = request.find("\r\n", 0);
	if (crlf_index == string::npos)
		throw std::runtime_error("request line syntax error");
	request_line.parse(string(request.begin(), request.begin() + crlf_index));
	// cout << "= = > Request Line: " << string(request.begin(), request.begin() + crlf_index) << '\n';
	crlf_index += 2;
	index_dcrlf = request.find("\r\n\r\n", 0);
	if (index_dcrlf == string::npos)
		throw std::runtime_error("request ctlf syntax error");
	while (index_tmp != index_dcrlf)
	{
		index_tmp = request.find("\r\n", crlf_index);
		if (index_tmp == string::npos)
			throw std::runtime_error("request headers syntax error");
		request_header.parse(string(request.begin() + crlf_index,
							  request.begin() + index_tmp));
		// cout << "= = > Request Header: " << string(request.begin() + crlf_index, request.begin() + index_tmp) << '\n';
		crlf_index = index_tmp + 2;
		index_tmp = request.find("\r\n", crlf_index);
	}
	if (index_tmp == string::npos)
		throw std::runtime_error("request headers syntax error");
	request_header.parse(string(request.begin() + crlf_index,
							 request.begin() + index_tmp));
	// cout << "= = > rest: " << std::distance(request.begin() + index_tmp + 4,
								   // request.end()) << '\n';
	if (request_line.getMethod() == "POST")
	{
		// size_t											body_size;
		// char*											end;
		map<const string, const string>::const_iterator	it;
	
		index_tmp += 4;
		it = request_header.getHeaderData().find("Content-Length");
		if (it == request_header.getHeaderData().end())
			throw std::runtime_error("no content length is provided");
		// body_size = std::strtol(it->second.c_str(), &end, 0);
		// cout << "crlf index: " << index_dcrlf << '\n';
		// cout << "body size: " << body_size << '\n';
		// cout << "request size: " << request.size() << '\n';
		// body.assign(request.begin() + index_dcrlf,
		// 	  request.begin() + index_dcrlf + body_size);
		body.assign(request.begin() + index_tmp,
								   request.end());
	}
}

string	Request::requestExec()
{
	try
	{
		Response	response(*this);

		if (!request_line.getMethod().compare("DELETE"))
			response.DELETEResource();
		else if (!request_line.getMethod().compare("GET"))
			response.GETResource();
		// else if (!request_line.getMethod().compare("POST"))
		// 	response.POSTResource();
		return (response.getResponse());
	}
// handel backward rethrowing
	catch (std::exception &e)
	{
		cout << "Exception: " << e.what() << '\n';
		return ("");
	}
}
