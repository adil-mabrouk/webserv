#include "Response.hpp"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <stdexcept>

Response::Response()
{
	autoindex = true;
	root_path = "/home/amabrouk/webserv/";
	index_file = "index.html";
}

void	Response::statusCode401()
{
	pair<string, string>	tmp_headers[4];

	tmp_headers[0] = make_pair("Server: ", "webserv");
	tmp_headers[1] = make_pair("Date: ", fillDate(time(NULL)));
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
	tmp_headers[1] = make_pair("Date: ", fillDate(time(NULL)));
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
	tmp_headers[1] = make_pair("Date: ", fillDate(time(NULL)));
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
	tmp_headers[1] = make_pair("Date: ", fillDate(time(NULL)));
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
	dirent*	directory;

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
		statusCode403();
	else
		body.append("</pre><hr></body>\n</html>");
}

void	Response::fillFileBody(int fd)
{
	char	*buff;
	int		file_size;

	file_size = std::stoi((headers.end() - 2)->second);
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

	fd = open((path + "/" + index_file).c_str(), O_RDONLY);
	if (fd == -1)
	{
		if (errno == ENOENT)
		{
			if (autoindex)
			{
				DIR*	dir;

				dir = opendir(path.c_str());
				if (!dir)
				{
					if (errno == ENOENT)
						statusCode404();
					else
						statusCode403();
				}
				else
				{
					try
					{
						status_code = 200, reason_phrase.assign("OK");
						headers.push_back(make_pair("Server: ", "webserv"));
						headers.push_back(make_pair("Date: ", fillDate(time(NULL))));
						headers.push_back(make_pair("Content-Type: ", "text/html"));
						fillDirBody(path, dir);
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
				statusCode403();
		}
		else
			statusCode403();
	}
	else
	{
		if (!stat((path + "/" + index_file).c_str(), &st))
			GETFile(path + "/" + index_file, fd, &st);
		else
			statusCode404();
	}
}

void	Response::GETFile(string path, int fd, struct stat *st)
{
	if (fd == -1)
	{
		if (errno == ENOENT)
			statusCode404();
		else
			statusCode403();
	}
	else
	{
		ostringstream	oss;

		try
		{
			status_code = 200, reason_phrase.assign("OK");
			headers.push_back(make_pair("Server: ", "webserv"));
			headers.push_back(make_pair("Date: ", fillDate(time(NULL))));
			headers.push_back(make_pair("Content-Type: ", fillContentType(path)));
			oss << st->st_size;
			headers.push_back(make_pair("Content-Length: ", oss.str()));
			headers.push_back(make_pair("Last-Modified: ", fillDate(st->st_mtim.tv_sec)));
			fillFileBody(fd);
		}
		catch (std::exception &e)
		{
			close(fd);
			throw;
		}
		close(fd);
	}
}

void	Response::GETResource(string path)
{
	struct stat	st;

	path = root_path + path;
 	if (!stat(path.c_str(), &st))
	{
		if (S_ISREG(st.st_mode))
			GETFile(path, open(path.c_str(), O_RDONLY), &st);
		else if (S_ISDIR(st.st_mode))
			GETDir(path);
		else
// should test nginx behavior for this one
			statusCode403();
	}
	else
		statusCode404();
}

// must verify if the program has the right to delete the resource?
void	Response::DELETEResource(string path)
{
	struct stat	st;

// open the path first
	path = root_path + path;
	if (stat(path.c_str(), &st))
		statusCode404();
	else
	{
		if (S_ISDIR(st.st_mode))
			statusCode403();
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
				if (unlink(path.c_str()))
					statusCode403();
				else
// check the headers
					status_code = 200, reason_phrase.assign("OK");
			}
			else
				statusCode401();
		}
		else
			statusCode403();
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

string	Response::fillContentType(string path)
{
	map<string, string>				content_types;
	map<string, string>::iterator	it_types;
	string							extension;
	string::reverse_iterator		it_extension;

    content_types.insert(make_pair("html", "text/html"));
    content_types.insert(make_pair("htm", "text/html"));
    content_types.insert(make_pair("text/html", "shtml;"));
    content_types.insert(make_pair("css", "text/css"));
    content_types.insert(make_pair("xml", "text/xml"));
    content_types.insert(make_pair("gif", "image/gif"));
    content_types.insert(make_pair("jpeg", "image/jpeg"));
    content_types.insert(make_pair("jpg", "image/jpeg"));
	content_types.insert(make_pair("js", "application/javascript"));
	content_types.insert(make_pair("atom", "application/atom+xml"));
	content_types.insert(make_pair("rss", "application/rss+xml"));
	content_types.insert(make_pair("mml", "text/mathml"));
	content_types.insert(make_pair("txt", "text/plain"));
	content_types.insert(make_pair("jad", "text/vnd.sun.j2me.app-descriptor"));
	content_types.insert(make_pair("wml", "text/vnd.wap.wml"));
	content_types.insert(make_pair("htc", "text/x-component"));
	content_types.insert(make_pair("avif", "image/avif"));
	content_types.insert(make_pair("png", "image/png"));
	content_types.insert(make_pair("svg", "image/svg+xml"));
	content_types.insert(make_pair("svgz", "image/svg+xml"));
	content_types.insert(make_pair("tif", "image/tiff"));
	content_types.insert(make_pair("tiff", "image/tiff"));
	content_types.insert(make_pair("wbmp", "image/vnd.wap.wbmp"));
	content_types.insert(make_pair("webp", "image/webp"));
	content_types.insert(make_pair("ico", "image/x-icon"));
	content_types.insert(make_pair("jng", "image/x-jng"));
	content_types.insert(make_pair("bmp", "image/x-ms-bmp"));
	content_types.insert(make_pair("woff", "font/woff"));
	content_types.insert(make_pair("woff2", "font/woff2"));
	content_types.insert(make_pair("jar", "application/java-archive"));
	content_types.insert(make_pair("war", "application/java-archive"));
	content_types.insert(make_pair("ear", "application/java-archive"));
	content_types.insert(make_pair("json", "application/json"));
	content_types.insert(make_pair("hqx", "application/mac-binhex40"));
	content_types.insert(make_pair("doc", "application/msword"));
	content_types.insert(make_pair("pdf", "application/pdf"));
	content_types.insert(make_pair("ps", "application/postscript"));
	content_types.insert(make_pair("eps", "application/postscript"));
	content_types.insert(make_pair("ai", "application/postscript"));
	content_types.insert(make_pair("rtf", "application/rtf"));
	content_types.insert(make_pair("m3u8", "application/vnd.apple.mpegurl"));
	content_types.insert(make_pair("kml", "application/vnd.google-earth.kml+xml"));
	content_types.insert(make_pair("kmz", "application/vnd.google-earth.kmz"));
	content_types.insert(make_pair("xls", "application/vnd.ms-excel"));
	content_types.insert(make_pair("eot", "application/vnd.ms-fontobject"));
	content_types.insert(make_pair("ppt", "application/vnd.ms-powerpoint"));
	content_types.insert(make_pair("odg", "application/vnd.oasis.opendocument.graphics"));
	content_types.insert(make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
	content_types.insert(make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
	content_types.insert(make_pair("odt", "application/vnd.oasis.opendocument.text"));
	content_types.insert(make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	content_types.insert(make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	content_types.insert(make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	content_types.insert(make_pair("wmlc", "application/vnd.wap.wmlc"));
	content_types.insert(make_pair("wasm", "application/wasm"));
	content_types.insert(make_pair("7z", "application/x-7z-compressed"));
	content_types.insert(make_pair("cco", "application/x-cocoa"));
	content_types.insert(make_pair("jardiff", "application/x-java-archive-diff"));
	content_types.insert(make_pair("jnlp", "application/x-java-jnlp-file"));
	content_types.insert(make_pair("run", "application/x-makeself"));
	content_types.insert(make_pair("pl", "application/x-perl"));
	content_types.insert(make_pair("pm", "application/x-perl"));
	content_types.insert(make_pair("prc", "application/x-pilot"));
	content_types.insert(make_pair("pdb", "application/x-pilot"));
	content_types.insert(make_pair("rar", "application/x-rar-compressed"));
	content_types.insert(make_pair("rpm", "application/x-redhat-package-manager"));
	content_types.insert(make_pair("sea", "application/x-sea"));
	content_types.insert(make_pair("swf", "application/x-shockwave-flash"));
	content_types.insert(make_pair("sit", "application/x-stuffit"));
	content_types.insert(make_pair("tcl", "application/x-tcl"));
	content_types.insert(make_pair("tk", "application/x-tcl"));
	content_types.insert(make_pair("der", "application/x-x509-ca-cert"));
	content_types.insert(make_pair("pem", "application/x-x509-ca-cert"));
	content_types.insert(make_pair("crt", "application/x-x509-ca-cert"));
	content_types.insert(make_pair("xpi", "application/x-xpinstall"));
	content_types.insert(make_pair("xhtml", "application/xhtml+xml"));
	content_types.insert(make_pair("xspf", "application/xspf+xml"));
	content_types.insert(make_pair("zip", "application/zip"));
	content_types.insert(make_pair("bin", "application/octet-stream"));
	content_types.insert(make_pair("exe", "application/octet-stream"));
	content_types.insert(make_pair("dll", "application/octet-stream"));
	content_types.insert(make_pair("deb", "application/octet-stream"));
	content_types.insert(make_pair("dmg", "application/octet-stream"));
	content_types.insert(make_pair("iso", "application/octet-stream"));
	content_types.insert(make_pair("img", "application/octet-stream"));
	content_types.insert(make_pair("msi", "application/octet-stream"));
	content_types.insert(make_pair("msp", "application/octet-stream"));
	content_types.insert(make_pair("msm", "application/octet-stream"));
	content_types.insert(make_pair("mid", "audio/midi"));
	content_types.insert(make_pair("midi", "audio/midi"));
	content_types.insert(make_pair("kar", "audio/midi"));
	content_types.insert(make_pair("mp3", "audio/mpeg"));
	content_types.insert(make_pair("ogg", "audio/ogg"));
	content_types.insert(make_pair("m4a", "audio/x-m4a"));
	content_types.insert(make_pair("ra", "audio/x-realaudio"));
	content_types.insert(make_pair("3gpp", "video/3gpp"));
	content_types.insert(make_pair("3gp", "video/3gpp"));
	content_types.insert(make_pair("ts", "video/mp2t"));
	content_types.insert(make_pair("mp4", "video/mp4"));
	content_types.insert(make_pair("mpeg", "video/mpeg"));
	content_types.insert(make_pair("mpg", "video/mpeg"));
	content_types.insert(make_pair("ogv", "video/ogg"));
	content_types.insert(make_pair("mov", "video/quicktime"));
	content_types.insert(make_pair("webm", "video/webm"));
	content_types.insert(make_pair("flv", "video/x-flv"));
	content_types.insert(make_pair("m4v", "video/x-m4v"));
	content_types.insert(make_pair("mkv", "video/x-matroska"));
	content_types.insert(make_pair("mng", "video/x-mng"));
	content_types.insert(make_pair("asx", "video/x-ms-asf"));
	content_types.insert(make_pair("asf", "video/x-ms-asf"));
	content_types.insert(make_pair("wmv", "video/x-ms-wmv"));
	content_types.insert(make_pair("avi", "video/x-msvideo"));
	it_extension = find(path.rbegin(), path.rend(), '.');
	if (it_extension == path.rend())
		return ("text/plain");
	extension.assign(it_extension.base(), path.end());
	it_types = content_types.find(extension);
	if (it_types != content_types.end())
		return (it_types->second);
	else
		return ("text/plain");
}

