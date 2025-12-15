#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../Request/Request.hpp"
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <ctime>
#include <sstream>
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>

using std::string;
using std::find;
using std::cout;
using std::vector;
using std::map;
using std::make_pair;
using std::pair;
using std::time;
using std::time_t;
using std::ostringstream;

class	Request;

// still need to canonicalize the path
class	Response
{
private:
	bool							autoindex;
	string							root_path;
	string							index_file;

	Request							request;
	int								status_code;
	string							reason_phrase;
	vector< pair<string, string> >	headers;
	string							body;
public:
	Response();
	Response(const Request&);

	string	fillDate(time_t);
	static string	fillContentType(string, int);
	string	fillLastModified(string);
	void	fillFileBody(int);
	void	fillDirBody(string, DIR*);

	void	statusCode400();
	void	statusCode401();
	void	statusCode403();
	void	statusCode404();
	void	statusCode500();
	void	statusCode501();

	void	GETDir(string);
	void	GETFile(string, int, struct stat*);
	void	GETResource();
	void	DELETEResource();

	const string							getResponse() const;
	int										getStatusCode() const;
	const string							getReasonPhrase() const;
	const vector< pair<string, string> >	getHeaders() const;
	const string							getBody() const;
};

#endif

//
//<html>
//<head><title>Index of /</title></head>
//<body>
//<h1>Index of /</h1><hr><pre><a href="../">../</a>
//<a href="dir1/">dir1/</a>                                              24-Nov-2025 14:04                   -
//<a href="index.nginx-debian.html">index.nginx-debian.html</a>                            24-Nov-2025 13:57                 612
//</pre><hr></body>
//</html>
