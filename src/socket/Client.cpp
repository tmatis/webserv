/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:21:45 by mamartin          #+#    #+#             */
/*   Updated: 2021/11/02 14:27:59 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*** F_POLLFD *****************************************************************/

f_pollfd::f_pollfd(const std::string& filename, int fd, int event, \
const std::string& data, bool cgi) : name(filename), data(data), cgi_file(cgi)
{
	pfd.fd		= fd;
	pfd.events	= event; // fd is read only
}

f_pollfd::operator pollfd(void) const
{
	return (pfd);
}

string_response::string_response(void) :
	data(""), written(0) {}

/*** CLIENT *******************************************************************/

Client::Client(void) :
	write_trials(0), last_request(), _state(PENDING_REQUEST), _files(), _route(NULL), _n_files(0) {}

int
Client::connect(int host_fd)
{
	socklen_t	size = sizeof(_addr);

	// create connected socket
	_fd = accept(host_fd, (sockaddr*)&_addr, &size);
	if (_fd == -1 || TCP_Socket::set_non_blocking(*this) == -1)
		return (-1);
	last_request = time(NULL); // get time of connection to handle timeout later
	return (0);
}

Client::operator pollfd() const
{
	pollfd	pfd;

	pfd.fd		= _fd;
	pfd.events	= 0;
	if (this->_state == WAITING_ANSWER || this->_state == PENDING_REQUEST)
		pfd.events = this->_state == PENDING_REQUEST ? POLLIN : POLLOUT;
	pfd.revents	= 0;
	return (pfd);
}

/*** GETTERS ******************************************************************/

client_state
Client::state(void) const
{
	return (_state);
}

const Route*
Client::rules(void) const
{
	return (_route);
}

HTTPRequest&
Client::request(void)
{
	return (_request);
}

HTTPResponse&
Client::response(void)
{
	return (_response);
}

std::vector<f_pollfd*>&
Client::files(void)
{
	return (_files);
}

int&
Client::files_number(void)
{
	return (_n_files);
}

str_response&
Client::string_response(void)
{
	return (_str);
}

/*** SETTERS ******************************************************************/

void
Client::state(client_state st)
{
	_state = st;
}

void
Client::rules(const Route* rules)
{
	_route = rules;
}

void
Client::add_file(f_pollfd* fpfd)
{
	_files.push_back(fpfd);
	_n_files++;
}

void
Client::clear(void)
{
	_state			= PENDING_REQUEST;
	_route			= NULL;
	_n_files		= 0;
	_str.written 	= 0;
	write_trials	= 0;
	
	_files.clear();
	_response.clear();
	_request.clear();
	_str.data.clear();
}
