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
	string	uri("/kapouet/pouic/toto/pouet");
	string	path("/kapouet");
	string	root("/tmp/www");

	if (!uri.find(path, 0))
	{
		uri.erase(0, path.size());
		uri = root + uri;
	}
	cout << uri << '\n';
}
