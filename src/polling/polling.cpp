/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:03:40 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/08 20:51:59 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#	include "polling.hpp"

PollClass::PollClass(int const &timeout) : \
_timeout(timeout), _server_list(), _pfd_list() {}
PollClass::PollClass(PollClass const &cp) { *this = cp; }
PollClass::~PollClass() {}

PollClass			&PollClass::operator=(PollClass const &rhs)
{
	this->_timeout = rhs._timeout;
	this->_pfd_list = rhs._pfd_list;
	this->_server_list = rhs._server_list;
	return (*this);
}

int					PollClass::polling()
{
	this->update_pfd();
	return (poll(&(this->_pfd_list[0]), this->_pfd_list.size(), this->_timeout));
}

void				PollClass::add_server(server &new_server)
{
	this->_server_list.push_back(&new_server);
	this->_server_data_size.push_back(std::make_pair(new_server._clients.size(), \
	new_server._files_fds.size()));
	this->_pfd_list.insert(this->_pfd_list.begin() + \
	(this->_pfd_list.empty() ? 0 : this->_server_list.size()), \
	this->_server_list.back()->get_fd);

	this->_pfd_list.insert(this->_pfd_list.end(), new_server._clients.begin(), \
	new_server._clients.end());
	this->_pfd_list.insert(this->_pfd_list.end(), \
	new_server._files_fds.begin(), new_server._files_fds.end());
}

void				PollClass::remove_server(server *server)
{
	unsigned int			index = 0;
	unsigned int			data_offset = 0;

	for ( ; index < this->_server_list.size() ; index++)
		if (this->_server_list[index] == *server)
			break ;
	if (index == this->_server_list.size())
		return ;
	for (unsigned int i = 0 ; i < index ; i++)
		data_offset += this->_server_data_size[i].first + \
		this->_server_data_size[i].second;
	data_offset += this->_server_list.size();
	
	this->_pfd_list.erase(this->_pfd_list.begin() + data_offset, \
	this->_pfd_list.begin() + data_offset + \
	this->_server_data_size[index].first + this->_server_data_size[index].second);
	this->_pfd_list.erase(this->_pfd_list.begin() + index);
	this->_server_list.erase(this->_server_list.begin() + index);
	this->_server_data_size.erase(this->_server_data_size.begin() + index);
}

PollClass::pfd		PollClass::make_pollfd(int fd, int events)
{
	pfd		spfd;

	spfd.fd = fd;
	spfd.events = events;
	spfd.revents = 0;
	return (spfd);
}

void				PollClass::update_server_pfd(unsigned int index)
{
	unsigned int	i = 0;
	unsigned int	offset = 0;
	unsigned int	new_cfd_size = this->_server_list[index]->_clients.size();
	unsigned int	new_fst_size = this->_server_list[index]->_files_fds.size();

	for (unsigned int i = 0 ; i < index ; i++)
		offset += this->_server_data_size[i].first + this->_server_data_size[i].second;
	offset += this->_server_list.size();
	try {
		this->_pfd_list.resize(offset + new_cfd_size + new_fst_size);
	} catch (std::exception &e) { throw ; }
	while (i < new_cfd_size)
	{
		if (this->_pfd_list[offset + i] == this->_server_list[index]->_clients[i])
		{
			switch (this->_server_list[index]->_clients[i].state)
			{
				case (1) :
					this->_pfd_list[offset + i].events = POLLOUT;
					break ;
				case (2) :
					this->_pfd_list[offset + i].events = POLLIN;
					break ;
				default:
					this->_pfd_list[offset + i].events = 0;
					break ;
			}
		}
		else
		{
			if (i < this->_server_data_size[index].first)
			{
				this->_pfd_list.erase(this->_pfd_list.begin() + offset + i);
				this->_server_data_size[index].first -= 1;
				continue ;
			}
			else
			{
				this->_pfd_list.insert(this->_pfd_list.begin() + offset + i, \
				this->_server_list[index]->_clients[i]);
			}
		}
		i++;
	}
	offset += new_cfd_size;
	i = 0;
	while (i < new_fst_size)
	{
		if (this->_pfd_list[offset + i] != this->_server_list[index]->_files_fds[i])
		{
			if (i < this->_server_data_size[index].second)
			{
				this->_pfd_list.erase(this->_pfd_list.begin() + offset + i);
				this->_server_data_size[index].first -= 1;
				continue ;
			}
			else
			{
				this->_pfd_list.insert(this->_pfd_list.begin() + offset + i, \
				this->_server_list[index]->_files_fds[i]);
			}
		}
		i++;
	}
	this->_server_data_size[index].first = new_cfd_size;
	this->_server_data_size[index].second = new_fst_size;
}

void				PollClass::update_pfd()
{
	for (unsigned int i = 0 ; i < this->_server_list.size() ; i++)
		this->update_server_pfd(i);
}