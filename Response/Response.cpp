#include "Response.hpp"
#include <cstdlib>

Response::Response() {}

Response::Response(const Request& request, const LocationConfig& location_config)
{
	this->request = request;
	this->location_config = location_config;
}

Response::Response(const LocationConfig& location_config)
	: location_config(location_config) {}

void	Response::statusCode301(const string& location_header)
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "156");
	tmp_headers[3] = make_pair("Location: ", location_header);
	status_code = 301, reason_phrase.assign("Moved Permanently");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>301 Moved Permanently</title></head>\n\
<body>\n\
<center><h1>301 Moved Permanently</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode302(const string& location_header)
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "132");
	tmp_headers[3] = make_pair("Location: ", location_header);
	status_code = 302, reason_phrase.assign("Moved Temporarily");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>302 Found</title></head>\n\
<body>\n\
<center><h1>302 Found</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}
void	Response::statusCode400()
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "144");
	status_code = 400, reason_phrase.assign("Bad Request");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>400 Bad Request</title></head>\n\
<body>\n\
<center><h1>400 Bad Request</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode401()
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "166");
	status_code = 401, reason_phrase.assign("Unauthorized");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>401 Authorization Required</title></head>\n\
<body>\n\
<center><h1>401 Authorization Required</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode403()
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "140");
	status_code = 403, reason_phrase.assign("Forbidden");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>403 Forbidden</title></head>\n\
<body>\n\
<center><h1>403 Forbidden</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode404()
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "140");
	status_code = 404, reason_phrase.assign("Not Found");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>404 Not Found</title></head>\n\
<body>\n\
<center><h1>404 Not Found</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode500()
{
	pair<string, string>			tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "182");
	status_code = 500, reason_phrase.assign("Internal Server Error");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>500 Internal Server Error</title></head>\n\
<body>\n\
<center><h1>500 Internal Server Error</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

void	Response::statusCode501()
{
	pair<string, string>			tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(std::time(NULL)));
	tmp_headers[2] = make_pair("Content-Type: ", "text/html");
	tmp_headers[3] = make_pair("Content-Length: ", "152");
	status_code = 501, reason_phrase.assign("Not Implemented");
	headers.assign(tmp_headers, tmp_headers + 4);
	body.assign("<html>\n\
<head><title>501 Not Implemented</title></head>\n\
<body>\n\
<center><h1>501 Not Implemented</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
}

string	Response::fillDate(time_t time1)
{
	const char	week_days[7][4] = {"Mon", "Tue", "Wed", "Thu",
		"Fri", "Sat", "Sun"};
	const char	months[12][4] = {"Jan", "Feb", "Mar", "Apr",
		"May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	string	date;
	ostringstream	oss;

	std::tm* time2 = std::gmtime(&time1);

	if (!time2->tm_wday)
		date.assign(week_days[6]).append(", ");
	else
		date.assign(week_days[time2->tm_wday - 1]).append(", ");

	oss << time2->tm_mday;
	date.append(oss.str()).append(" ")
		.append(months[time2->tm_mon]).append(" ");
	oss.str(""), oss.clear();

	oss << time2->tm_year + 1900;
	date.append(oss.str()).append(" ");
	oss.str(""), oss.clear();

	oss << time2->tm_hour;
	if (time2->tm_hour < 10)
		date.append("0");
	date.append(oss.str()).append(":");
	oss.str(""), oss.clear();

	oss << time2->tm_min;
	if (time2->tm_min < 10)
		date.append("0");
	date.append(oss.str()).append(":");
	oss.str(""), oss.clear();

	oss << time2->tm_sec;
	if (time2->tm_sec < 10)
		date.append("0");
	date.append(oss.str()).append(" GMT");
	return (date);
}

void	Response::fillDirBody(string path, DIR* dir)
{
	std::stringstream	ss;
	dirent*				directory;

	errno = 0;
	directory = readdir(dir);
	body.assign("<html>\n<head><title>Indexof ")
		.append(path).append("</title></head>\n<body>\n<h1>Index of ")
		.append(path).append("</h1><hr><pre><a href=\"../\">../</a>\n");
	while (directory)
	{
		if (!strcmp(directory->d_name, ".")
			|| !strcmp(directory->d_name, ".."))
		{
			directory = readdir(dir);
			continue ;
		}
		if (directory->d_type == DT_DIR)
			body.append("<a href=\"").append(directory->d_name)
				.append("/\">").append(directory->d_name).append("/</a>\n");
		else
			body.append("<a href=\"").append(directory->d_name)
				.append("\">").append(directory->d_name).append("</a>\n");
		directory = readdir(dir);
	}
	if (!directory && errno == EBADF)
		throw 403;//statusCode403();
	else
		body.append("</pre><hr></body>\n</html>");
	ss << body.size();
	if (!headers[headers.size() - 1].first.compare("Content-Length: "))
		headers[headers.size() - 1].second = ss.str();
	else
		headers.push_back(make_pair("Content-Length: ", ss.str()));
}

void	Response::fillFileBody(int fd)
{
	char	*end;
	char	*buff;
	size_t	file_size;

	file_size = std::strtol((headers.end() - 2)->second.c_str(), &end, 0);
	buff = new char[file_size + 1];
	try
	{
		if (read(fd, buff, file_size) == -1)
			throw std::runtime_error("");
		buff[file_size] = '\0';
		body.assign(buff);
	}
	catch (std::exception &e)
	{
		delete[] buff;
		throw;
	}
	delete[] buff;
}

void	Response::GETDir(string path)
{
	int			fd;
	struct stat	st;

	for (vector<string>::iterator it = location_config.index.begin();
		it != location_config.index.end(); it++)
	{
		fd = open((path + "/" + *it).c_str(), O_RDONLY);
		if (fd != -1)
			break ;
	}
	// fd = open((path + "/" + index_file).c_str(), O_RDONLY);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			if (location_config.autoindex)
			{
				DIR*	dir;

				dir = opendir(path.c_str());
				if (!dir)
				{
					if (errno == ENOENT)
						throw 404;//statusCode404();
					else
						throw 403;//statusCode403();
				}
				else
				{
					try
					{
						// stringstream	ss;

						status_code = 200, reason_phrase.assign("OK");
						headers.push_back(make_pair("Server: ", "webserv"));
						headers.push_back(make_pair("Date: ", fillDate(std::time(NULL))));
						headers.push_back(make_pair("Content-Type: ", "text/html"));
						fillDirBody(path, dir);
						// ss << body.size();
						// headers.push_back(make_pair("Content-Length: ", ss.str()));
					}
					catch (std::exception& e)
					{
						closedir(dir);
						throw;
					}
					closedir(dir);
				}
			}
			else
				throw 403;//statusCode403();
		}
		else
			throw 403;//statusCode403();
	}
	else
	{
		for (vector<string>::iterator it = location_config.index.begin();
		it != location_config.index.end(); it++)
		{
			if (!stat((path + "/" + *it).c_str(), &st))
			{
				GETFile(path + "/" + *it, fd, &st);
				break ;
			}
			else
				if (it + 1 == location_config.index.end())
				throw 404;//statusCode404();
		}
	}
}

void	Response::GETFile(string path, int fd, struct stat *st)
{
	if (fd == -1)
	{
		if (errno == ENOENT)
			throw 404;//statusCode404();
		else
			throw 403;//statusCode403();
	}
	else
	{
		ostringstream	oss;

		try
		{
			size_t	index;

			status_code = 200, reason_phrase.assign("OK");
			headers.push_back(make_pair("Server: ", "webserv"));
			headers.push_back(make_pair("Date: ", fillDate(std::time(NULL))));
			index = path.find(".");
			if (index == string::npos)
				headers.push_back(make_pair("Content-Type: ", "text/plain"));
			else
				headers.push_back(make_pair("Content-Type: ",
								fillContentType(string(path.begin() + index + 1, path.end()), 0)));
			oss << st->st_size;
			headers.push_back(make_pair("Content-Length: ", oss.str()));
			headers.push_back(make_pair("Last-Modified: ", fillDate(st->st_mtim.tv_sec)));
			fillFileBody(fd);
		}
		catch (std::exception &e)
		{
			close(fd);
			fd = -1;
			throw;
		}
		close(fd);
		fd = -1;
	}
}

void	Response::GETResource()
{
	struct stat	st;
	string		path;

	path = request.request_line.getURI();
 	if (!stat(path.c_str(), &st))
	{
		if (S_ISREG(st.st_mode))
			GETFile(path, open(path.c_str(), O_RDONLY), &st);
		else if (S_ISDIR(st.st_mode))
			GETDir(path);
		else
// should test nginx behavior for this one
			throw 403;//statusCode403();
	}
	else
		throw 404;//statusCode404();
}

// must verify if the program has the right to delete the resource?
void	Response::DELETEResource()
{
	struct stat	st;
	string		path;

// open the path first
	path = request.request_line.getURI();
	if (stat(path.c_str(), &st))
		throw 404;//statusCode404();
	else
	{
		if (S_ISDIR(st.st_mode))
			throw 403;//statusCode403();
		else if (S_ISREG(st.st_mode))
		{
			string parent;
			string::reverse_iterator	it;

			it = find(path.rbegin(), path.rend(), '/');
//		how about if there is no '/'
			if (it == path.rend())
				parent = "..";
			else
				parent.assign(path.begin(), it.base());
// X_OK?
			if (!access(parent.c_str(), W_OK | X_OK))
			{
				if (std::remove(path.c_str()))
					throw 403;//statusCode403();
				else
// check the headers
					status_code = 200, reason_phrase.assign("OK");
			}
			else
				throw 401;//statusCode401();
		}
		else
			throw 403;//statusCode403();
	}
}

int	Response::getStatusCode() const
{
	return (status_code);
}

const string	Response::getReasonPhrase() const
{
	return (reason_phrase);
}

const vector< pair<string, string> >	Response::getHeaders() const
{
	return (headers);
}

const string	Response::getBody() const
{
	return (body);
}

const string	Response::getResponse() const
{
	ostringstream	os;
	string			response;

	os << status_code;
	response.append("HTTP/1.1 ").append(os.str())
		.append(" ").append(reason_phrase).append("\r\n");
	for (vector< pair<string, string> >::const_iterator it = headers.begin();
		it != headers.end(); it++)
		response.append(it->first).append(it->second).append("\r\n");
	response.append("\r\n").append(body);
	return (response);
}

string	Response::fillContentType(string content_type, int flag)
{
	pair<string, string>			mime_types[112] =
		{make_pair("html", "text/html"), 
			make_pair("htm", "text/html"), 
			make_pair("text/html", "shtml;"), 
			make_pair("css", "text/css"), 
			make_pair("xml", "text/xml"), 
			make_pair("gif", "image/gif"), 
			make_pair("jpeg", "image/jpeg"), 
			make_pair("jpg", "image/jpeg"), 
			make_pair("js", "application/javascript"), 
			make_pair("atom", "application/atom+xml"), 
			make_pair("rss", "application/rss+xml"), 
			make_pair("mml", "text/mathml"), 
			make_pair("txt", "text/plain"), 
			make_pair("jad", "text/vnd.sun.j2me.app-descriptor"), 
			make_pair("wml", "text/vnd.wap.wml"), 
			make_pair("htc", "text/x-component"), 
			make_pair("avif", "image/avif"), 
			make_pair("png", "image/png"), 
			make_pair("svg", "image/svg+xml"), 
			make_pair("svgz", "image/svg+xml"), 
			make_pair("tif", "image/tiff"), 
			make_pair("tiff", "image/tiff"), 
			make_pair("wbmp", "image/vnd.wap.wbmp"), 
			make_pair("webp", "image/webp"), 
			make_pair("ico", "image/x-icon"), 
			make_pair("jng", "image/x-jng"), 
			make_pair("bmp", "image/x-ms-bmp"), 
			make_pair("woff", "font/woff"), 
			make_pair("woff2", "font/woff2"), 
			make_pair("jar", "application/java-archive"), 
			make_pair("war", "application/java-archive"), 
			make_pair("ear", "application/java-archive"), 
			make_pair("json", "application/json"), 
			make_pair("hqx", "application/mac-binhex40"), 
			make_pair("doc", "application/msword"), 
			make_pair("pdf", "application/pdf"), 
			make_pair("ps", "application/postscript"), 
			make_pair("eps", "application/postscript"), 
			make_pair("ai", "application/postscript"), 
			make_pair("rtf", "application/rtf"), 
			make_pair("m3u8", "application/vnd.apple.mpegurl"), 
			make_pair("kml", "application/vnd.google-earth.kml+xml"), 
			make_pair("kmz", "application/vnd.google-earth.kmz"), 
			make_pair("xls", "application/vnd.ms-excel"), 
			make_pair("eot", "application/vnd.ms-fontobject"), 
			make_pair("ppt", "application/vnd.ms-powerpoint"), 
			make_pair("odg", "application/vnd.oasis.opendocument.graphics"), 
			make_pair("odp", "application/vnd.oasis.opendocument.presentation"), 
			make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"), 
			make_pair("odt", "application/vnd.oasis.opendocument.text"), 
			make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"), 
			make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"), 
			make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"), 
			make_pair("wmlc", "application/vnd.wap.wmlc"), 
			make_pair("wasm", "application/wasm"), 
			make_pair("7z", "application/x-7z-compressed"), 
			make_pair("cco", "application/x-cocoa"), 
			make_pair("jardiff", "application/x-java-archive-diff"), 
			make_pair("jnlp", "application/x-java-jnlp-file"), 
			make_pair("run", "application/x-makeself"), 
			make_pair("pl", "application/x-perl"), 
			make_pair("pm", "application/x-perl"), 
			make_pair("prc", "application/x-pilot"), 
			make_pair("pdb", "application/x-pilot"), 
			make_pair("rar", "application/x-rar-compressed"), 
			make_pair("rpm", "application/x-redhat-package-manager"), 
			make_pair("sea", "application/x-sea"), 
			make_pair("swf", "application/x-shockwave-flash"), 
			make_pair("sit", "application/x-stuffit"), 
			make_pair("tcl", "application/x-tcl"), 
			make_pair("tk", "application/x-tcl"), 
			make_pair("der", "application/x-x509-ca-cert"), 
			make_pair("pem", "application/x-x509-ca-cert"), 
			make_pair("crt", "application/x-x509-ca-cert"), 
			make_pair("xpi", "application/x-xpinstall"), 
			make_pair("xhtml", "application/xhtml+xml"), 
			make_pair("xspf", "application/xspf+xml"), 
			make_pair("zip", "application/zip"), 
			make_pair("bin", "application/octet-stream"), 
			make_pair("exe", "application/octet-stream"), 
			make_pair("dll", "application/octet-stream"), 
			make_pair("deb", "application/octet-stream"), 
			make_pair("dmg", "application/octet-stream"), 
			make_pair("iso", "application/octet-stream"), 
			make_pair("img", "application/octet-stream"), 
			make_pair("msi", "application/octet-stream"), 
			make_pair("msp", "application/octet-stream"), 
			make_pair("msm", "application/octet-stream"), 
			make_pair("mid", "audio/midi"), 
			make_pair("midi", "audio/midi"), 
			make_pair("kar", "audio/midi"), 
			make_pair("mp3", "audio/mpeg"), 
			make_pair("ogg", "audio/ogg"), 
			make_pair("m4a", "audio/x-m4a"), 
			make_pair("ra", "audio/x-realaudio"), 
			make_pair("3gpp", "video/3gpp"), 
			make_pair("3gp", "video/3gpp"), 
			make_pair("ts", "video/mp2t"), 
			make_pair("mp4", "video/mp4"), 
			make_pair("mpeg", "video/mpeg"), 
			make_pair("mpg", "video/mpeg"), 
			make_pair("ogv", "video/ogg"), 
			make_pair("mov", "video/quicktime"), 
			make_pair("webm", "video/webm"), 
			make_pair("flv", "video/x-flv"), 
			make_pair("m4v", "video/x-m4v"), 
			make_pair("mkv", "video/x-matroska"), 
			make_pair("mng", "video/x-mng"), 
			make_pair("asx", "video/x-ms-asf"), 
			make_pair("asf", "video/x-ms-asf"), 
			make_pair("wmv", "video/x-ms-wmv"), 
			make_pair("avi", "video/x-msvideo")};
	for (int index = 0; index < 112; index++)
		if (!flag && content_type == mime_types[index].first)
			return (mime_types[index].second);
		else if (flag && content_type == mime_types[index].second)
			return (mime_types[index].first);
	return ((!flag) ? "text/plain" : "txt");
}

