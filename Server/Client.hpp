#pragma once


#include "Server.hpp"

class Client {
	private:
		int					_fd;				// client socket
		std::string			_requestBuffer;	 // stores incoming data
		std::string			_responseBuffer;	// stores outgoing data
		bool				_requestComplete;   // flag for completed request
		size_t				_bytesSent;		 // track how much of response is sent
		bool				_readyToWrite;	  // ready for EPOLLOUT or POLLOUT

	public:
		Client(int fd);
		~Client();

		// Getters
		int					getFd() const;
		bool				requestComplete() const;
		bool				readyToWrite() const;
		const std::string&	getRequest() const;
		const std::string&	getResponse() const;

		// Setters
		void				setResponse(const std::string &response);
		void				markReadyToWrite(bool state);

		// I/O methods
		bool				readRequest();	 // read from socket → fills _requestBuffer
		bool				writeResponse();   // write from _responseBuffer → socket
		bool				isDone() const;	 // check if response fully sent
		// Request handling
		void reset();		   // reset buffers for next request
};