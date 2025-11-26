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
	// request_exec();
}

string	Request::request_exec()
{
	string res;
	try
	{
		
		if (!request_line.getMethod().compare("DELETE"))
			response.DELETEResource(request_line.getURI());
		else if (!request_line.getMethod().compare("GET"))
			response.GETResource("/home/amabrouk/Desktop/webserv");

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
