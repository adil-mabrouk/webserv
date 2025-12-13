#include "RequestHeader.hpp"

//RequestHeader::RequestHeader()
//{
	//hf_data_parse.insert("Authorization", isAuthorization);
	//hf_data_parse.insert("FROM", isFrom);
	//hf_data_parse.insert("If-Modified-Since", isIfModSin);
	//hf_data_parse.insert("Referer", isReferer);
	//hf_data_parse.insert("User-Agent", isUserAgent);
	//hf_data_parse.insert("Accept", isAccept);
	//hf_data_parse.insert("Accept-Charset", isAccCha);
	//hf_data_parse.insert("Accept-Encoding", isAccEnc);
	//hf_data_parse.insert("Accept-Language", isAccLan);
	//hf_data_parse.insert("Content-Language", isConLan);
	//hf_data_parse.insert("Link", isLink);
	//hf_data_parse.insert("MIME-Version", isMIMEVersion);
	//hf_data_parse.insert("Retry-After", isRetryAfter);
	//hf_data_parse.insert("Title", isTitle);
	//hf_data_parse.insert("URI", isURI);
//}

// revise this parsing
void	RequestHeader::parse(string str)
{
	string::iterator	it;
	string				header_name;
	string::iterator	it_header;

	it_header = str.begin();
	while (it_header != str.end())
	{
		if (str.find("\r\n") == string::npos)
			std::cout << "here\n", exit(1);
		string	header(it_header, it_header + str.find("\r\n"));

		it = find(header.begin(), header.end(), ':');
		if (it == header.end())
			throw std::runtime_error("header field separator error");
		for (string::iterator it_tmp = header.begin();
			it_tmp != it; it_tmp++)
			if (*it_tmp != '-' && (*it_tmp < 'a' || *it_tmp > 'z')
				&& (*it_tmp < 'A' || *it_tmp > 'Z'))
				throw std::runtime_error("header field name error");
		header_name.assign(header.begin(), it);
		if (*(++it) != ' ')
			throw std::runtime_error("header field value error");
		header_data.insert(std::make_pair(header_name, string(++it, header.end())));
		str.assign(str.begin() + str.find("\r\n") + 2, str.end());
		it_header = str.begin();
	}
	// std::cout << header_name << ": " << string(it, str.end()) << '\n';
}

bool	RequestHeader::isGeneralHeader(string str)
{
	return (!str.compare("Date") || !str.compare("Pragma"));
}

bool	RequestHeader::isRequestHeader(string str)
{
	return (!str.compare("Authorization")
			|| !str.compare("From")
			|| !str.compare("If-Modified-Since")
			|| !str.compare("Referer")
			|| !str.compare("User-Agent"));
}

bool	RequestHeader::isEntityHeader(string str)
{
	return (!str.compare("Allow")
			|| !str.compare("Content-Encoding")
			|| !str.compare("Content-Length")
			|| !str.compare("Content-Type")
			|| !str.compare("Expires")
			|| !str.compare("Last-Modified")
			|| !str.compare("extension-Header"));
}

const map<const string, const string>&	RequestHeader::getHeaderData()
{
	return (header_data);
}
