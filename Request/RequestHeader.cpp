#include "RequestHeader.hpp"

void	RequestHeader::parse(string str)
{
	size_t	index;
	size_t	index2;

	index2 = 0;
	while (index2 < str.size())
	{
		index = str.find(':', index2);
		if (index == string::npos)
			throw 400;
		string	field_name(string(str.begin() + index2, str.begin() + index++));
		if (!isFieldName(field_name))
			throw 400;
		if (str[index++] != ' ')
			throw 400;
		index2 = str.find("\r\n", index);
		if (index2 == string::npos)
			throw 400;
		string	field_body(string(str.begin() + index, str.begin() + index2));
		// std::cout << '|' << field_name << "|: |" << field_body << "|\n";
		index2 += 2;
		if (field_name == "Content-Length" && !field_body.size())
			throw 400;
		if (field_name == "Content-Length")
			for (size_t content_length_index = 0;
				content_length_index < field_body.size();
				content_length_index++)
				if (field_body[content_length_index] < '0'
					|| field_body[content_length_index] > '9')
					throw 400;
		header_data.insert(std::make_pair(field_name, field_body));
	}
}

bool	isCTL(char c)
{
	return (c == 127 || (c >= 0 && c <= 31));
}

bool	RequestHeader::isFieldName(string str) const
{
	if (!str.size())
		return (false);
	for (size_t index = 0; index < str.size(); index++)
	{
		if (str[index] == ' ' ||
			str[index] == ':' ||
			isCTL(str[index]))
			return (false);
	}
	return (true);
}

const map<const string, const string>&	RequestHeader::getHeaderData()
{
	return (header_data);
}
