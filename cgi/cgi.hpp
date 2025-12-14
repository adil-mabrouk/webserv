#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <iostream>
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/wait.h>    
#include <fcntl.h>      
#include <signal.h>      
#include <cstdlib>       
#include <cstring>      
#include <ctime>        

class CGI
{
    private:
        std::string								_scriptPath;		
        std::string								_method;			
        std::string								_queryString;		
        std::string								_body;				
        std::map<std::string, std::string>		_headers;			
        std::string								_workingDir;		
        int										_timeout;			
        
        std::string intToString(int n)
        {
            if (n == 0)
                return "0";
                
            std::string result = "";
            bool negative = false;
            
            if (n < 0)
            {
                negative = true;
                n = -n;
            }
            
            while (n > 0)
            {
                result = char('0' + (n % 10)) + result;
                n = n / 10;
            }
            
            if (negative)
                result = "-" + result;
                
            return result;
        }

        std::string getHeader(const std::string& key)
        {
            std::map<std::string, std::string>::iterator it = _headers.find(key);
            if (it != _headers.end())
                return it->second;
            return "";
        }

        /*
        ** Setup CGI environment variables
        ** 
        ** - REQUEST_METHOD: GET, POST, etc.
        ** - QUERY_STRING: Data after ? in URL
        ** - CONTENT_LENGTH: Size of request body
        ** - SCRIPT_FILENAME: Full path to script
        */
        std::map<std::string, std::string> setupEnvironment()
        {
            std::map<std::string, std::string> env;
            
            env["REQUEST_METHOD"] = _method;
            env["SCRIPT_FILENAME"] = _scriptPath;
            env["QUERY_STRING"] = _queryString;
            env["CONTENT_LENGTH"] = intToString(_body.length());
            env["SERVER_PROTOCOL"] = "HTTP/1.0";
            env["GATEWAY_INTERFACE"] = "CGI/1.0";
            env["REDIRECT_STATUS"] = "200";  
            
            std::string contentType = getHeader("Content-Type");
            if (!contentType.empty())
                env["CONTENT_TYPE"] = contentType;
            else
                env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
            
            for (std::map<std::string, std::string>::iterator it = _headers.begin();
                it != _headers.end(); ++it)
            {
                std::string key = "HTTP_" + it->first;
                
                for (size_t i = 0; i < key.length(); i++)
                {
                    if (key[i] == '-')
                        key[i] = '_';
                    else if (key[i] >= 'a' && key[i] <= 'z')
                        key[i] = key[i] - 32; 
                }
                
                env[key] = it->second;
            }
            
            return env;
        }

        char** mapToEnvArray(const std::map<std::string, std::string>& env)
        {
            char** envp = new char*[env.size() + 1];
            size_t i = 0;
            
            for (std::map<std::string, std::string>::const_iterator it = env.begin();
                it != env.end(); ++it)
            {
                std::string envVar = it->first + "=" + it->second;
                
                envp[i] = new char[envVar.length() + 1];
                std::strcpy(envp[i], envVar.c_str());
                i++;
            }
            
            envp[i] = NULL; 
            return envp;
        }

        /*
        ** Free environment array
        */
        void freeEnvArray(char** envp)
        {
            if (!envp)
                return;
                
            for (size_t i = 0; envp[i] != NULL; i++)
                delete[] envp[i];
            delete[] envp;
        }

        /*
        ** Determine which interpreter to use based on file extension
        ** Examples:
        ** - .php  -> /usr/bin/php-cgi
        ** - .py   -> /usr/bin/python3
        */
        std::string getCGIInterpreter(const std::string& scriptPath)
        {
            size_t dotPos = scriptPath.rfind('.');
            if (dotPos == std::string::npos)
                return "";  
            
            std::string ext = scriptPath.substr(dotPos);
            
            if (ext == ".php")
                return "/usr/bin/php-cgi";
            else if (ext == ".py")
                return "/usr/bin/python3";
            
            return ""; 
        }

        /*
        ** Format CGI output into proper HTTP response
        */
        std::string formatCGIOutput(const std::string& output)
        {
            if (output.find("HTTP/") == 0)
                return output;
            
            if (output.find("\r\n\r\n") != std::string::npos ||
                output.find("\n\n") != std::string::npos)
            {
                return "HTTP/1.0 200 OK\r\n" + output;
            }
            return "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" + output;
        }
    public:
        CGI() : _timeout(30)
	    {
	    }

        // ~CGI();
        void setScriptPath(const std::string& path)		{ _scriptPath = path; }
        void setMethod(const std::string& method)		{ _method = method; }
        void setQueryString(const std::string& query)	{ _queryString = query; }
        void setBody(const std::string& body)			{ _body = body; }
        void setWorkingDir(const std::string& dir)		{ _workingDir = dir; }
        void setTimeout(int seconds)					{ _timeout = seconds; }
        void setHeader(const std::string& key, const std::string& value)
	    {
            _headers[key] = value;
	    }   
        std::string execute();

};

#endif