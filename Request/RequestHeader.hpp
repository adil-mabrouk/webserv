#ifndef REQUESTHEADER_HPP
#define REQUESTHEADER_HPP

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

using std::string;
using std::map;

class	RequestHeader
{
private:
	// typedef	void (*parse)(string);
	// map<const string, parse>		hf_data_parse;
	map<const string, const string>	header_data;
public:
	// RequestHeader();

	void	parse(string);

	bool	isGeneralHeader(string);
	bool	isRequestHeader(string);
	bool	isEntityHeader(string);
	// bool isAutorization(string);
	// bool isFrom(string);
	// bool isIfModSin(string);
	// bool isReferer(string);
	// bool isUserAgent(string);
	// bool isAccept(string);
	// bool isAccCha(string);
	// bool isAccEnc(string);
	// bool isAccLan(string);
	// bool isConLan(string);
	// bool isLink(string);
	// bool isMIMEVersion(string);
	// bool isRetryAfter(string);
	// bool isTitle(string);
	// bool isURI(string);
};

#endif
