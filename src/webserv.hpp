#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <poll.h>

# include "http/HTTPHeader.hpp"
# include "http/HTTPRequest.hpp"
# include "http/HTTPResponse.hpp"
# include "polling/polling.hpp"
# include "socket/Client.hpp"
# include "socket/Listener.hpp"
# include "socket/Server.hpp"
# include "socket/TCP_Socket.hpp"

int event_pollin(std::vector<struct pollfd> &pollfd,
				 std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				 std::vector<struct pollfd>::iterator it);

int event_pollout(std::vector<struct pollfd> &pollfd,
				 std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				 std::vector<struct pollfd>::iterator it);

int event_pollhup(std::vector<struct pollfd> &pollfd,
				  std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				  std::vector<struct pollfd>::iterator it);

int event_pollerr(std::vector<struct pollfd> &pollfd,
				  std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				  std::vector<struct pollfd>::iterator it);

int event_pollnval(std::vector<struct pollfd> &pollfd,
				   std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				   std::vector<struct pollfd>::iterator it);

#endif
