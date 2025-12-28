#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <ctime>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <sstream>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

using std::string;
using std::cout;
using std::cerr;
using std::map;
using std::find;
using std::pair;
using std::make_pair;
using std::vector;

bool	find_location(vector<string> names, string uri)
{
	while (uri.size())
	{
		cout << "searching for " << uri << '\n';
		for (vector<string>::iterator	it = names.begin();
			it != names.end(); it++)
			if (!it->compare(uri))
				cout << "found at " << std::distance(names.begin(), it) << " " + *it, exit(0);
		if (uri[uri.size() - 1] == '/')
			uri.erase(uri.size() - 1, 1);
		else
		{
			size_t	index;
			
			index = uri.rfind('/', uri.size());
			if (index == string::npos)
				return (cout << "not found\n", false);
			uri.erase(uri.begin() + index + 1, uri.end());
		}
	}
	return (cout << "not found\n", false);
}

int	main(int ac, char *av[])
{
	string str("<html>\n\
<head><title>302 Found</title></head>\n\
<body>\n\
<center><h1>302 Found</h1></center>\n\
<hr><center>Webserv (Ubuntu)</center>\n\
</body>\n\
</html>");
	cout << str.size() << '\n';
}
