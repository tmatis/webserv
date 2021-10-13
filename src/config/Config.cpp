/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:51:01 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/13 15:44:28 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// Route::Route(void) : root("/var/www"), location("/"), autoindex(false), \
// cgi_extension(""), upload_path("") {}

Config::Config(void) : MasterConfig(), server_names(), body_limit(0) {}

Config::Config(MasterConfig const &master) : MasterConfig(master), body_limit(0) {}

void	Config::construct(std::string &config_str)
{
	std::vector<std::pair<std::string, std::string> >	raw_data;
	std::pair<std::string, std::string>					parsing_res;
	unsigned int										i = 0;
	bool												listen_flag = false;

	try {
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
			MasterConfig::keep_only_printable_chars(raw_data[i].second);
			if (!this->fill_var(raw_data[i]))
				break ;
			i++;
		}
		if (!listen_flag)
			throw std::invalid_argument("missing listen directive in server block");
	} catch (std::exception &e) { throw ; }
}

int	Config::fill_var(std::pair<std::string, std::string> const &var_pair)
{
	unsigned long					i = 0;
	std::vector<std::string>		values;
	std::string						values_raw = var_pair.second;

	std::string		str_args[11] = {"index", "error_page", "root", "autoindex", \
	"listen", "server_name", "redirection", "body_limit", "user", "error_log", \
	"max_simultaneous_clients"};
	void (Config::*func_args[8])(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values) = {&Config::set_index_paths, \
	&Config::set_error_pages, &Config::set_root, &Config::set_autoindex, \
	&Config::set_listen, &Config::set_server_names, &Config::set_redirection, \
	&Config::set_body_limit};
	while (!values_raw.empty())
	{
		i = values_raw.find(' ');
		if (i == std::string::npos)
			i = values_raw.size();
		values.push_back(values_raw.substr(0, i));
		values_raw.erase(0, i);
	}
	for (i = 0 ; i < 11 ; i++)
	{
		if (var_pair.first == str_args[i])
		{
			if (i > 7)
			{
				std::cerr << "config > \'" << var_pair.first << "\' : this \
				argument doesn't exist in the server scope (ignored)" << std::endl;
				return (1);
			}
			(this->*func_args[i])(var_pair, values);
			return (1);
		}
	}
	return (0);
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
				inet_pton(AF_INET, "0.0.0.0", &this->address);
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
			if (inet_pton(AF_INET, values[0].c_str(), &this->address))
			{
				std::istringstream(values[0]) >> port;
				if (port >= 0 && port <= 65535)
				{
					this->port = htons(port);
					return ;
				}
			}
		}
	}
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
		throw std::invalid_argument("you must provide at least one index");
}

void	Config::set_redirection(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	int					i = 0;
	std::string			path = var_pair.second;

	if (values.size() > 2 && MasterConfig::is_there_only_digits(values[0]))
	{
		path.erase(0, values[0].size() + 1);
		std::istringstream(values[0]) >> i;
		this->redirection = std::make_pair(i, path);
	}
	else
		throw std::invalid_argument("you must provide a number and a path for redirection");
}

void	Config::set_body_limit(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0]))
		std::istringstream(values[0]) >> this->body_limit;
	else
		throw std::invalid_argument("max simultaneous clients must be a positive number");
}

void
Config::add_default_route(void)
{
	Route	r;

	// set default route values
	r.methods.push_back("GET");
	r.methods.push_back("POST");
	r.methods.push_back("DELETE");
	r.default_pages.push_back("index.html");

	routes.push_back(r);
}
