/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 14:43:37 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/12 21:54:09 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "MasterConfig.hpp"

MasterConfig::MasterConfig() : _flags(0), _autoindex(false), \
_max_simultaneous_clients(-1), _user(), _error_log(), \
_default_mime("application/octet-stream"), _mime_types(), \
_index_paths(), _error_pages() {}
MasterConfig::~MasterConfig()
{
	if (this->_error_log.is_open())
		this->_error_log.close();
}

void \
MasterConfig::fill_var(std::pair<std::string, std::string> const &var_pair)
{
	unsigned int					i = 0;
	std::vector<std::string>		values;
	std::string						values_raw = var_pair.second;
	struct passwd					*psw = NULL;

	while (!values_raw.empty())
	{
		i = values_raw.find(' ');
		if (i == std::string::npos)
			i = values_raw.size();
		values.push_back(values_raw.substr(0, i));
		values_raw.erase(0, i);
	}
	if (var_pair.first == "autoindex")
	{
		if (values.size() == 1 && values[0] == "on")
			this->_autoindex = true;
		else if (values.size() == 1 && values[0] == "off")
			this->_autoindex = false;
		else
			throw std::invalid_argument("autoindex must be on or off");
	}
	else if (var_pair.first == "max_simultaneous_clients")
	{
		if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0]))
			std::istringstream(values[0]) >> this->_max_simultaneous_clients;
		else
			throw std::invalid_argument("max simultaneous clients must be a positive number");
	}
	else if (var_pair.first == "user")
	{
		if (values.size() == 1)
		{
			psw = getpwnam(values[0].c_str());
			if (!psw)
				throw std::invalid_argument("can't access to the specified user");
			if (setuid(psw->pw_uid) == -1)
				throw std::invalid_argument("can't set the given user to current user");
			this->_user = values[0];
		}
		else
			throw std::invalid_argument("you must provide a user to user param");
	}
	else if (var_pair.first == "error_log")
	{
		if (values.size())
		{
			this->_error_log.open(values_raw, std::ofstream::out | std::ofstream::app);
			if (!this->_error_log.is_open())
				throw std::invalid_argument("can't open the provided file");
			std::cerr.rdbuf(this->_error_log.rdbuf());
		}
		else
			throw std::invalid_argument("you must provide a path for error_log");
	}
	else if (var_pair.first == "index")
	{
		if (values.size())
		{
			for ( ; i < values.size() ; i++)
				this->_index_paths.insert(values[i]);
		}
		else
			throw std::invalid_argument("you must provide at least one index");
	}
	else if (var_pair.first == "error_page")
	{
		if (values.size() == 2 && )
		{

		}
		else
			throw std::invalid_argument("you must provide a number and a path for error_page");
	}
}

void		MasterConfig::construct(std::string const &config_path)
{
	std::vector<std::pair<std::string, std::string> >	raw_data;
	std::ifstream										config_fd;
	std::string											config_str;
	std::pair<std::string, std::string>					parsing_res;
	int													length = 0;
	unsigned int										i = 0;
	char												*buffer = NULL;

	try {
		config_fd.open(config_path, std::ifstream::in);
		if (!config_fd.is_open())
			throw std::invalid_argument("can't open the provided file");
		config_fd.seekg(0, config_fd.end);
		length = config_fd.tellg();
		config_fd.seekg(0, config_fd.beg);
		buffer = new char[length];
		config_fd.read(buffer, length);
		config_str = buffer;
		parsing_res = this->extract_key_value(config_str);
		while (!parsing_res.first.empty())
		{
			raw_data.push_back(parsing_res);
			parsing_res = this->extract_key_value(config_str);
		}
		while (i < raw_data.size())
		{
			MasterConfig::keep_only_printable_chars(raw_data[i].first);
			if (raw_data[i].first == "server")
				break ;
			MasterConfig::keep_only_printable_chars(raw_data[i].second);
			this->fill_var(raw_data[i]);
		}
	} catch (std::exception &e)
	{
		if (config_fd.is_open())
			config_fd.close();
		delete[] buffer;
		throw ;
	}
	config_fd.close();
	delete[] buffer;


}

std::pair<std::string, std::string> \
MasterConfig::extract_key_value(std::string &line)
{
	std::pair<std::string, std::string>		ret;
	bool									braces = false;
	unsigned int							i = 0;
	int										find_res = 0;
	unsigned char							count = 0;

	if (line.empty())
		return (ret);
	while (count < 4)
	{
		i = 0;
		find_res = 0;
		if (count == 3 && line[i] == '{')
			braces = true;
		for ( ; i < line.size() ; i++)
		{
			if (braces && line[i] == '{')
				find_res++;
			if (braces && line[i] == '}')
				find_res--;

			if ((!braces) && count && (line[i] == '#' || line[i] == '\n'))
				break ;
			if ((!(count % 2) && ((line[i] != '\n') && !isspace(line[i]))) || \
			(count == 1 && isspace(line[i])) || \
			(count == 3 && !braces && line[i] == ';') ||
			(count == 3 && braces && line[i] == '}' && !find_res))
				break ;
		}
		if (!count && line[i] == '#')
		{
			find_res = line.find('\n');
			if (find_res == std::string::npos)
				find_res = line.size();
			else
				find_res += 1;
			line.erase(0, find_res);
			find_res = 0;
			continue ;
		}
		if (i)
		{
			if (line[i] == '#' || line[i] == '\n')
				throw std::invalid_argument("misplaced comment or new line");
			if (count % 2)
			{
				if (count == 1)
					ret.first = line.substr(0, i);
				else
				{
					if (i >= line.size() || (line[i] != ';' && line[i] != '}'))
						throw std::invalid_argument("missing terminaison");
					if (braces && line[i] == '}')
						ret.second = line.substr(1, i - 1);
					else if (!braces && line[i] == ';')
						ret.second = line.substr(0, i);
					i++;
				}
			}
			line.erase(0, i);
		}
		count++;
	}
	return (ret);
}

void		MasterConfig::keep_only_printable_chars(std::string &edit)
{
	unsigned int		i = 0;

	while (i < edit.size())
	{
		if (!isprint(edit[i]))
		{
			edit.erase(i, 1);
			continue ;
		}
		i++;
	}
}

bool		MasterConfig::is_there_only_digits(std::string const &edit)
{
	unsigned int		i = 0;

	while (i < edit.size())
	{
		if (!isdigit(edit[i]))
			return (false);
		i++;
	}
	return (true);
}