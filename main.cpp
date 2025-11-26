#include "Request/Request.hpp"
#include "Response/Response.hpp"

int main(int ac, char *av[])
{
	Request request;
	(void)ac;

	try
	{
		request.request_parsing(av[1]);
	}
	catch (std::exception& e)
	{
		cerr << e.what() << '\n';
		return (1);
	}
}
