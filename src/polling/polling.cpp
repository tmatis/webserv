/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:03:40 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/30 11:07:05 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#	include "polling.hpp"

PollClass *PollClass::pc_ptr = NULL;

PollClass::PollClass() {}
PollClass::PollClass(int const &timeout) : \
_timeout(timeout), _pfd_list(), _server_list()
{
	if (fcntl(0, F_SETFL, O_NONBLOCK) == -1 || \
	fcntl(1, F_SETFL, O_NONBLOCK) == -1 || \
	fcntl(2, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(strerror(errno));
	this->_pfd_list.push_back(this->make_pollfd(0, POLLIN));
	this->_pfd_list.push_back(this->make_pollfd(1, POLLOUT));
	this->_pfd_list.push_back(this->make_pollfd(2, POLLOUT));
}
PollClass::PollClass(PollClass const &cp) { *this = cp; }
PollClass::~PollClass() {}

PollClass			&PollClass::operator=(PollClass const &rhs)
{
	this->_timeout = rhs._timeout;
	this->_pfd_list = rhs._pfd_list;
	this->_server_list = rhs._server_list;
	this->_server_data_size = rhs._server_data_size;
	return (*this);
}

int					PollClass::polling()
{
	this->update_pfd();
	return (poll(&(this->_pfd_list[0]), this->_pfd_list.size(), this->_timeout));
}

void				PollClass::add_server(server &new_server, unsigned int const \
&crash_flag, unsigned int crash_pos)
{
	try {
		if (crash_flag == 5)
			throw std::runtime_error("too many tries");
		if (crash_pos < 1)
		{
			this->_server_list.push_back(&new_server);
			crash_pos++;
		}
		if (crash_pos < 2)
		{
			this->_server_data_size.push_back(std::make_pair(new_server.get_clients().size(), \
			new_server.get_files().size()));
			crash_pos++;
		}
		if (crash_pos < 3)
		{
			this->_pfd_list.insert(this->_pfd_list.begin() + BASE_OFFSET + this->_server_list.size() - 1, \
			PollClass::make_pollfd(this->_server_list.back()->get_listener().fd()));
			crash_pos++;
		}
		if (crash_pos < 4)
		{
			if (new_server.get_clients().size())
				this->_pfd_list.insert(this->_pfd_list.end(), new_server.get_clients().begin(), \
				new_server.get_clients().end());
			crash_pos++;
		}
		if (new_server.get_files().size())
			for (size_t i = 0; i < new_server.get_files().size(); i++)
				this->_pfd_list.insert(this->_pfd_list.end(), *new_server.get_files()[i]);
	} catch (std::exception &e) {
		if (crash_flag == 5)
			throw std::bad_alloc();
		this->add_server(new_server, crash_flag + 1, crash_pos);
	}
}

void				PollClass::remove_server(server *server)
{
	unsigned int			index = 0;
	unsigned int			data_offset = BASE_OFFSET;

	for ( ; index < this->_server_list.size() ; index++)
		if (this->_server_list[index] == server)
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
	this->_pfd_list.erase(this->_pfd_list.begin() + index + BASE_OFFSET);
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

void				PollClass::update_server_pfd(unsigned int const &index, \
unsigned int const &crash_flag)
{
	try {
		unsigned int	i = 0;
		unsigned int	offset = BASE_OFFSET;
		unsigned int	new_cfd_size = this->_server_list[index]->get_clients().size();
		unsigned int	new_fst_size = this->_server_list[index]->get_files().size();

		if (crash_flag == 5)
			throw std::runtime_error("5 tries failed");
		for (unsigned int i = 0 ; i < index ; i++)
			offset += this->_server_data_size[i].first + this->_server_data_size[i].second;
		offset += this->_server_list.size();
		if (this->_pfd_list.capacity() < offset + new_cfd_size + new_fst_size)
			this->_pfd_list.reserve(offset + new_cfd_size + new_fst_size);
		while (i < new_cfd_size)
		{
			if (i + offset < this->_pfd_list.size() && this->_pfd_list[offset + i].fd == \
			this->_server_list[index]->get_clients()[i].fd())
			{
				switch (this->_server_list[index]->get_clients()[i].state())
				{
					case (WAITING_ANSWER) :
						this->_pfd_list[offset + i].events = POLLOUT;
						break ;
					case (PENDING_REQUEST) :
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
					this->_server_list[index]->get_clients()[i]);
				}
			}
			i++;
		}
		while (i < this->_server_data_size[index].first)
		{
			this->_pfd_list.erase(this->_pfd_list.begin() + offset + i);
			this->_server_data_size[index].first -= 1;
		}
		offset += new_cfd_size;
		i = 0;
		while (i < new_fst_size)
		{
			if (i + offset >= this->_pfd_list.size() || this->_pfd_list[offset + i].fd != \
			this->_server_list[index]->get_files()[i]->pfd.fd)
			{
				if (i < this->_server_data_size[index].second)
				{
					this->_pfd_list.erase(this->_pfd_list.begin() + offset + i);
					this->_server_data_size[index].second -= 1;
					continue ;
				}
				else
				{
					this->_pfd_list.insert(this->_pfd_list.begin() + offset + i, \
					*this->_server_list[index]->get_files()[i]);
				}
			}
			i++;
		}
		while (i < this->_server_data_size[index].second)
		{
			this->_pfd_list.erase(this->_pfd_list.begin() + offset + i);
			this->_server_data_size[index].second -= 1;
		}
		this->_server_data_size[index].first = new_cfd_size;
		this->_server_data_size[index].second = new_fst_size;
	} catch (std::exception &e) {
		if (crash_flag == 5)
			throw std::bad_alloc();
		this->update_server_pfd(index, crash_flag + 1);
	}
}

void				PollClass::update_pfd()
{
	for (unsigned int i = 0 ; i < this->_server_list.size() ; i++)
		this->update_server_pfd(i);
}

int					PollClass::get_raw_revents(int const &fd, server *server)
{
	unsigned int		offset = 0;
	unsigned int		i = 0;

	if (server)
	{
		offset += this->_server_list.size();
		while (i < this->_server_list.size() && this->_server_list[i] != server)
		{
			offset += this->_server_data_size[i].first + this->_server_data_size[i].second;
			i++;
		}
		if (i == this->_server_list.size())
			return (-1);
	}
	while (offset < this->_pfd_list.size())
	{
		if (this->_pfd_list[offset].fd == fd)
			return (this->_pfd_list[offset].revents);
		offset++;
	}
	return (-1);
}

int					PollClass::get_raw_revents(client const &cl, server *server)
{ return (this->get_raw_revents(cl.fd(), server)); }

PollClass			*PollClass::get_pollclass()
{ return (pc_ptr); }
