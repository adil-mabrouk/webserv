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

void	RequestHeader::parse(string str)
{
	string::iterator	it;
	string				header_name;

	it = find(str.begin(), str.end(), ':');
	if (it == str.end())
		throw std::runtime_error("header field separator error");
	for (string::iterator it_tmp = str.begin();
			it_tmp != it; it_tmp++)
		if (*it_tmp != '-' && (*it_tmp < 'a' || *it_tmp > 'z')
				&& (*it_tmp < 'A' || *it_tmp > 'Z'))
			throw std::runtime_error("header field name error");
	header_name.assign(str.begin(), it);
	if (*(++it) != ' ')
		throw std::runtime_error("header field value error");
	header_data.insert(std::make_pair(header_name, string(++it, str.end())));
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
