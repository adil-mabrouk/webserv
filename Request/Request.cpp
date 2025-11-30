#include "Request.hpp"
#include <string>

void	Request::request_parsing(string request)
{
	size_t	crlf_index;
	size_t	index_tmp;

	crlf_index = request.find("\r\n", 0);
	if (crlf_index == string::npos)
		throw std::runtime_error("request line syntax error");
	request_line.parse(string(request.begin(), request.begin() + crlf_index));
	crlf_index += 2;
	index_tmp = 0;
	while (crlf_index != index_tmp)
	{
		index_tmp = request.find("\r\n", crlf_index);
		if (index_tmp == string::npos)
			throw std::runtime_error("request headers syntax error");
		request_header.parse(string(request.begin() + crlf_index,
							  request.begin() + index_tmp));
		crlf_index = index_tmp + 2;
		index_tmp = request.find("\r\n", crlf_index);
	}
}

string	Request::request_exec()
{
	string	res;

	try
	{
		if (!request_line.getMethod().compare("DELETE"))
			response.DELETEResource(request_line.getURI());
		else if (!request_line.getMethod().compare("GET"))
			response.GETResource(request_line.getURI());

		std::ostringstream os;

		os << response.getStatusCode();
		res.append("HTTP/1.1 ").append(os.str())
			.append(" ").append(response.getReasonPhrase()).append("\r\n");
		vector< pair<string, string> > headers = response.getHeaders();
		for (vector< pair<string, string> >::iterator it = headers.begin();
			it != headers.end(); it++)
			res.append(it->first).append(it->second).append("\r\n");
		res.append("\r\n");
		res.append(response.getBody());

		// cout << "HTTP/1.0 "
		// 	<< response.getStatusCode() << ' '
		// 	<< response.getReasonPhrase() << "\r\n";
		// headers = response.getHeaders();
		// for (vector< pair<string, string> >::iterator it = headers.begin();
		// 	it != headers.end(); it++)
		// 	cout << it->first << it->second << '\n';
		// cout << "\r\n";
		// cout << response.getBody() << "\r\n\r\n";
	}
	catch (std::exception &e)
	{
		cout << "Exception: " << e.what() << '\n';
	}
	return res;
}
