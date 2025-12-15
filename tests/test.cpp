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

int	main(int ac, char *av[])
{
	string body;

	body.assign("<html>\n\
<head><title>501 Not Implemented</title></head>\n\
<body>\n\
<center><h1>501 Not Implemented</h1></center>\n\
<hr><center>Webserv</center>\n\
</body>\n\
</html>");
	cout << body.size() << '\n';
}
