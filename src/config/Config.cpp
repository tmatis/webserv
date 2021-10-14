/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:51:01 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/14 11:57:45 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "MasterConfig.hpp"
#include "Route.hpp"

Config::Config(void) : MasterConfig(), \
methods(this->_methods_supported), server_names(), body_limit(0) {}
Config::Config(MasterConfig const &master) : MasterConfig(master), \
methods(this->_methods_supported), body_limit(0) {}
Config::Config(Config const &cp)  { *this = cp; }
Config::~Config() {}

Config					&Config::operator=(Config const &rhs)
{
	this->MasterConfig::operator=(rhs);
	this->address = rhs.address;
	this->address_res = rhs.address_res;
	this->port = rhs.port;
	this->redirection = rhs.redirection;
	this->server_names = rhs.server_names;
	this->body_limit = rhs.body_limit;
	this->routes = rhs.routes;
	this->methods = rhs.methods;
	return (*this);
}

void	Config::construct(std::string &config_str)
{
	std::vector<std::pair<std::string, std::string> >	raw_data;
	std::pair<std::string, std::string>					parsing_res;
	unsigned int										i = 0;
	bool												listen_flag = false;

	try {
		std::cout << "x" << std::endl;
		parsing_res = this->extract_key_value(config_str);
		while (!parsing_res.first.empty())
		{
			raw_data.push_back(parsing_res);
			parsing_res = this->extract_key_value(config_str);
		}
		while (i < raw_data.size())
		{
			MasterConfig::keep_only_printable_chars(raw_data[i].first);
			if (raw_data[i].first == "listen")
				listen_flag = true;
			if (raw_data[i].first[0] == '_')
				break ;
			MasterConfig::keep_only_printable_chars(raw_data[i].second);
			this->fill_var(raw_data[i]);
			i++;
		}
		if (!listen_flag)
		{
			std::cerr << "config > the \'listen\' instruction is mandatory " << \
			"for a server (fatal error)" << std::endl;
			throw std::invalid_argument("missing listen directive in server block");
		}
		while (i < raw_data.size())
		{
			if (raw_data[i].first.size() == 1 || raw_data[i].first[0] != '_')
			{
				std::cerr << "config > \'" << raw_data[i].first << "\' : this " << \
				"is not a route block (ignored)" << std::endl;
				i++;
				continue ;
			}
			this->routes.push_back(Route(*this));
			this->routes.back().construct(raw_data[i]);
			i++;
		}
		if (this->routes.empty())
			this->routes.push_back(Route(*this));
	} catch (std::exception &e) { throw ; }
}

void	Config::fill_var(std::pair<std::string, std::string> const &var_pair)
{
	unsigned long					i = 0;
	std::vector<std::string>		values;
	std::string						values_raw = var_pair.second;

	std::string		str_args[10] = {"index", "error_page", "root", "autoindex", \
	"listen", "server_name", "redirection", "body_limit", "upload_files", "methods"};
	void (Config::*func_args[10])(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values) = {&Config::set_index_paths, \
	&Config::set_error_pages, &Config::set_root, &Config::set_autoindex, \
	&Config::set_listen, &Config::set_server_names, &Config::set_redirection, \
	&Config::set_body_limit, &Config::set_uploadfiles, &Config::set_methods};
	while (!values_raw.empty())
	{
		i = values_raw.find(' ');
		if (i == std::string::npos)
			i = values_raw.size();
		values.push_back(values_raw.substr(0, i));
		values_raw.erase(0, i + 1);
	}
	for (i = 0 ; i < 10 ; i++)
	{
		if (var_pair.first == str_args[i])
		{
			(this->*func_args[i])(var_pair, values);
			return ;
		}
	}
	std::cerr << "config > \'" << var_pair.first << "\' : this " << \
	"argument doesn't exist in the server scope (ignored)" << std::endl;
}

void	Config::set_listen(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	long int		port = 0;

	if (values.size() && values.size() < 3)
	{
		if (values.size() == 1)
		{
			if (values[0].find('.') != std::string::npos)
			{
				this->port = htons(80);
				if (inet_pton(AF_INET, values[0].c_str(), &this->address))
					return ;
			}
			else
			{
				inet_pton(AF_INET, "0.0.0.0", &this->address_res);
				this->address = "0.0.0.0";
				std::istringstream(values[0]) >> port;
				if (port >= 0 && port <= 65535)
				{
					this->port = htons(port);
					return ;
				}
			}
		}
		else
		{
			if (inet_pton(AF_INET, values[0].c_str(), &this->address_res))
			{
				this->address = values[0];
				std::istringstream(values[1]) >> port;
				if (port >= 0 && port <= 65535)
				{
					this->port = htons(port);
					return ;
				}
			}
		}
	}
	std::cerr << "config > \'" << var_pair.first << "\' : this this " << \
	"directive must be filled with a valid ip or port or both (fatal error)" << std::endl;
	throw std::invalid_argument("listen must be filled with either a valid ip or port or both");
}

void	Config::set_server_names(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	this->server_names.clear();
	if (values.size())
	{
		for (unsigned int i = 0 ; i < values.size() ; i++)
			this->server_names.insert(values[i]);
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs one value (ignored)" << std::endl;
}

void	Config::set_redirection(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	int					i = 0;
	std::string			path = var_pair.second;

	if (values.size() >= 2 && MasterConfig::is_there_only_digits(values[0]))
	{
		path.erase(0, values[0].size() + 1);
		std::istringstream(values[0]) >> i;
		this->redirection = std::make_pair(i, path);
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a number and a path (ignored)" << std::endl;
}

void	Config::set_body_limit(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0]))
		std::istringstream(values[0]) >> this->body_limit;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a positive value (ignored)" << std::endl;
}

void \
Config::set_methods(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	unsigned int i = 0;
	std::set<std::string>	new_set;

	(void)var_pair;
	this->_index_paths.clear();
	if (values.size())
	{
		for ( ; i < values.size() ; i++)
		{
			if (this->_methods_supported.find(values[i]) == this->_methods_supported.end())
			{
				std::cerr << "config > \'" << var_pair.first << "\' : the method \'" << \
				values[i] << "\' is either misspelled or not supported (ignored)" << std::endl;
				return ;
			}
			new_set.insert(values[i]);
		}
		this->methods = new_set;
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs at least one method as value (ignored)" << std::endl;
}