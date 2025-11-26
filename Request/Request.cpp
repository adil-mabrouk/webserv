#include "Request.hpp"

void	Request::request_parsing(string request)
{
	string::iterator	it;
	string::iterator	it_tmp;

	it = find(request.begin(), request.end(), '\r');
	if (it == request.end())
		throw std::runtime_error("request line error");
	request_line.parse(string(request.begin(), it));
	it++;
	while (*it == '\r')
	{
		// if (*(it + 1) == '\n')
			// break;
		it_tmp = it + 1;
		it = find(it_tmp, request.end(), '\r');
		if (it == request.end())
		{
			request_header.parse(string(it_tmp, it));
			break ;
		}
			// throw std::runtime_error("request header error");
		request_header.parse(string(it_tmp, it));
		it++;
	}
	request_exec();
}

void	Request::request_exec()
{
	try
	{
		if (!request_line.getMethod().compare("DELETE"))
			response.DELETEResource(request_line.getURI());
		else if (!request_line.getMethod().compare("GET"))
			response.GETResource(request_line.getURI());

		cout << "HTTP/1.0 "
			<< response.getStatusCode() << ' '
			<< response.getReasonPhrase() << "\r\n";
		vector< pair<string, string> > headers = response.getHeaders();
		for (vector< pair<string, string> >::iterator it = headers.begin();
			it != headers.end(); it++)
			cout << it->first << it->second << '\n';
		cout << "\r\n";
		cout << response.getBody() << "\r\n\r\n";
	}
	catch (std::exception &e)
	{
		cout << "Exception: " << e.what() << '\n';
	}
}
