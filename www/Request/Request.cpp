#include "Request.hpp"
#include "../Response/Response.hpp"

// string	Request::requestExec()
// {
// 	try
// 	{
// 		Response	response(*this);
// 
// 		if (!request_line.getMethod().compare("DELETE"))
// 			response.DELETEResource();
// 		else if (!request_line.getMethod().compare("GET"))
// 			response.GETResource();
// 		return (response.getResponse());
// 	}
// // handel backward rethrowing
// 	catch (std::exception &e)
// 	{
// 		cout << "Exception: " << e.what() << '\n';
// 		return ("");
// 	}
// }
