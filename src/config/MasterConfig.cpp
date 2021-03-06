/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 14:43:37 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/25 11:56:12 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>

#include "MasterConfig.hpp"
#include "Config.hpp"
#include "../utils/templates.hpp"

MasterConfig::MasterConfig() : _server_name_version("firewebserv/0.0"), \
_flags(0), _autoindex(false), \
_uploadfiles(false), _max_simultaneous_clients(-1), _timeout(15000), \
_user(), _error_log(), _old_cerr(),
_default_mime("application/octet-stream"), _mime_types(),
_index_paths(), _error_pages(), _upload_rights(S_IRUSR | S_IWUSR)
{
	this->_methods_supported.insert("GET");
	this->_methods_supported.insert("DELETE");
	this->_methods_supported.insert("POST");
}
MasterConfig::MasterConfig(MasterConfig const &cp) : \
_server_name_version("firewebserv/0.0") { *this = cp; }
MasterConfig::~MasterConfig()
{
	if (this->_error_log.is_open())
	{
		// std::cerr << std::endl;
		std::cerr.rdbuf(this->_old_cerr);
		this->_error_log.close();
	}
}

MasterConfig	&MasterConfig::operator=(MasterConfig const &rhs)
{
	this->_flags = rhs._flags;
	this->_autoindex = rhs._autoindex;
	this->_uploadfiles = rhs._uploadfiles;
	this->_max_simultaneous_clients = rhs._max_simultaneous_clients;
	this->_root = rhs._root;
	this->_default_mime = rhs._default_mime;
	this->_mime_types = rhs._mime_types;
	this->_index_paths = rhs._index_paths;
	this->_error_pages = rhs._error_pages;
	this->_methods_supported = rhs._methods_supported;
	this->_upload_rights = rhs._upload_rights;
	this->_old_cerr = rhs._old_cerr;
	this->_timeout = rhs._timeout;
	return (*this);
}

void \
MasterConfig::fill_var(std::pair<std::string, std::string> const &var_pair)
{
	unsigned long					i = 0;
	std::vector<std::string>		values;
	std::string						values_raw = var_pair.second;

	std::string const str_args[12] = {"upload_rights", "user", "error_log", "max_simultaneous_clients", "index", \
	"error_page", "root", "autoindex", "upload_files", "mime_types", "default_mime_type", "timeout"};

	typedef void (MasterConfig::*func_setter)
					(std::pair<std::string, std::string> const &var_pair,
					std::vector<std::string> const &values);

	func_setter const func_args[12] = {&MasterConfig::set_upload_rights, &MasterConfig::set_user, \
	&MasterConfig::set_error_log, &MasterConfig::set_max_simultaneous_clients, &MasterConfig::set_index_paths, \
	&MasterConfig::set_error_pages, &MasterConfig::set_root, &MasterConfig::set_autoindex, \
	&MasterConfig::set_uploadfiles, &MasterConfig::set_mime_types, &MasterConfig::set_default_mime_type, \
	&MasterConfig::set_timeout};

	while (!values_raw.empty())
	{
		i = values_raw.find(' ');
		if (i == std::string::npos)
			i = values_raw.size();
		values.push_back(values_raw.substr(0, i));
		values_raw.erase(0, i + 1);
	}
	for (i = 0 ; i < 12 ; i++)
	{
		if (var_pair.first == str_args[i])
		{
			(this->*func_args[i])(var_pair, values);
			return ;
		}
	}
	std::cerr << "config > \'" << var_pair.first << "\' : this argument doesn't exist in the global scope (ignored)" << std::endl;
}

void		MasterConfig::construct(std::string const &config_path)
{
	std::vector<std::pair<std::string, std::string> >	raw_data;
	std::ifstream										config_fd;
	std::string											config_str;
	std::pair<std::string, std::string>					parsing_res;
	char												*buffer = NULL;

	try {
		size_t length = 0;
		config_fd.open(config_path.c_str(), std::ifstream::in);
		if (!config_fd.is_open())
			throw std::invalid_argument("can't open the provided file");
		config_fd.seekg(0, config_fd.end);
		length = config_fd.tellg();
		config_fd.seekg(0, config_fd.beg);
		buffer = new char[length + 1];
		config_fd.read(buffer, length);
		buffer[config_fd.gcount()] = 0;
		config_str = buffer;
		MasterConfig::remove_comments(config_str);
		parsing_res = this->extract_key_value(config_str);
		while (!parsing_res.first.empty())
		{
			raw_data.push_back(parsing_res);
			parsing_res = this->extract_key_value(config_str);
		}
		unsigned int i = 0;
		while (i < raw_data.size())
		{
			MasterConfig::keep_only_printable_chars(raw_data[i].first);
			if (raw_data[i].first == "server")
				break ;
			MasterConfig::keep_only_printable_chars(raw_data[i].second);
			this->fill_var(raw_data[i]);
			i++;
		}
		if (i == raw_data.size())
		{
			std::cerr << "config > a server needs at least one server block to be launched (fatal error)" << std::endl;
			throw std::invalid_argument("can't launch a server w/out server block");
		}
		while (i < raw_data.size())
		{
			this->_configs.push_back(Config(*this));
			this->_configs.back().construct(raw_data[i].second);
			i++;
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
	unsigned char							count = 0;

	if (line.empty())
		return (ret);
	while (count < 4)
	{
		size_t i = 0;
		int	find_res = 0;
		if (count == 3 && line[i] == '{')
			braces = true;
		for ( ; i < line.size() ; i++)
		{
			if (braces && line[i] == '{')
				find_res++;
			if (braces && line[i] == '}')
				find_res--;

			if ((!braces) && count && (line[i] == '\n'))
				break ;
			if ((!(count % 2) && ((line[i] != '\n') && !isspace(line[i]))) || \
			(count == 1 && isspace(line[i])) || \
			(count == 3 && !braces && line[i] == ';') ||
			(count == 3 && braces && line[i] == '}' && !find_res))
				break ;
		}
		if (i)
		{
			if (line[i] == '\n')
				throw std::invalid_argument("misplaced new line");
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
		if (count == 3 && !i)
			throw std::invalid_argument("missing terminaison i");
		if (!count && line.empty())
			break ;
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

void		MasterConfig::remove_comments(std::string &edit)
{
	unsigned long i = edit.find('#');
	
	while (i != std::string::npos)
	{
		unsigned long y = edit.find('\n', i);
		if (y == std::string::npos)
			y = edit.size();
		edit.erase(i, y - i);
		i = edit.find('#');
	}
}

bool		MasterConfig::is_there_only_digits(std::string const &edit, bool const &or_dot)
{
	unsigned int		i = 0;
	bool				dot = false;

	while (i < edit.size())
	{
		if (!isdigit(edit[i]) && !or_dot)
			return (false);
		if (!isdigit(edit[i]) && or_dot && (edit[i] != '.' || dot))
			return (false);
		if (edit[i] == '.')
			dot = true;
		i++;
	}
	return (true);
}

void \
MasterConfig::set_error_pages(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	std::ifstream					error_page;
	char							*buffer;
	std::string						path = var_pair.second;

	if (values.size() >= 2 && MasterConfig::is_there_only_digits(values[0]))
	{
		path.erase(0, values[0].size() + 1);
		error_page.open(path.c_str(), std::ifstream::in);
		if (!error_page.is_open())
		{
			std::cerr << "config > \'" << var_pair.first << "\' : error page \'" << values[0] << \
			"\' - can't open the file \'" << path << "\' (ignored)" << std::endl;
			return ;
		}
		error_page.seekg(0, error_page.end);
		size_t i = error_page.tellg();
		error_page.seekg(0, error_page.beg);
		buffer = new char[i + 1];
		error_page.read(buffer, i);
		buffer[error_page.gcount()] = 0;
		i = 0;
		i = aton<size_t>(values[0]);
		this->_error_pages[i] = buffer;
		delete[] buffer;
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a number and a path (ignored)" << std::endl;
}

void \
MasterConfig::set_root(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	if (values.size())
		this->_root = var_pair.second;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a path (ignored)" << std::endl;
}

void \
MasterConfig::set_error_log(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	if (values.size())
	{
		this->_error_log.open(var_pair.second.c_str(), std::ofstream::out); // | std::ofstream::app);
		if (!this->_error_log.is_open())
			throw std::invalid_argument("can't open the provided file");
		this->_old_cerr = std::cerr.rdbuf(this->_error_log.rdbuf());
	}
	else
	{
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a path (fatal error)" << std::endl;
		throw std::invalid_argument("you must provide a path for error_log");
	}
}

void \
MasterConfig::set_user(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	struct passwd					*psw = NULL;
	(void)var_pair;

	if (values.size() == 1)
	{
		psw = getpwnam(values[0].c_str());
		if (!psw)
		{
			std::cerr << "config > \'" << var_pair.first << "\' : this " << \
			"directive needs a correct user (fatal error)" << std::endl;
			throw std::invalid_argument("can't access to the specified user");
		}
		if (setuid(psw->pw_uid) == -1)
		{
			std::cerr << "config > \'" << var_pair.first << "\' : this " << \
			"user can't be set - maybe sudo is needed - (fatal error)" << std::endl;
			throw std::invalid_argument("can't set the given user to current user");
		}
		this->_user = values[0];
	}
	else
	{
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a correct user (fatal error)" << std::endl;
		throw std::invalid_argument("you must provide a correct user to user param");
	}
}

void \
MasterConfig::set_max_simultaneous_clients(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0]))
		std::istringstream(values[0]) >> this->_max_simultaneous_clients;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a positive number (ignored)" << std::endl;
}

void \
MasterConfig::set_autoindex(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && values[0] == "on")
		this->_autoindex = true;
	else if (values.size() == 1 && values[0] == "off")
		this->_autoindex = false;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a <on-off> value (ignored)" << std::endl;
}

void \
MasterConfig::set_uploadfiles(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && values[0] == "on")
		this->_uploadfiles = true;
	else if (values.size() == 1 && values[0] == "off")
		this->_uploadfiles = false;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a <on-off> value (ignored)" << std::endl;
}

void \
MasterConfig::set_index_paths(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	this->_index_paths.clear();
	if (values.size())
	{
		for (size_t i = 0 ; i < values.size() ; i++)
			this->_index_paths.insert(values[i]);
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs at least one index as value (ignored)" << std::endl;
}

void \
MasterConfig::set_mime_types(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	std::ifstream						mime_types;
	std::map<std::string, std::string>	new_mime_tab;
	char								*buffer;
	std::string							tampon = var_pair.second;
	std::string							tampon2;

	if (values.size() >= 1)
	{
		mime_types.open(tampon.c_str(), std::ifstream::in);
		if (!mime_types.is_open())
		{
			std::cerr << "config > \'" << var_pair.first << "\' : " << \
			"can't open the file \'" << tampon << "\' (ignored)" << std::endl;
			return ;
		}
		mime_types.seekg(0, mime_types.end);
		unsigned long i = mime_types.tellg();
		mime_types.seekg(0, mime_types.beg);
		buffer = new char[i + 1];
		mime_types.read(buffer, i);
		buffer[mime_types.gcount()] = 0;
		i = 0;
		tampon = buffer;
		delete[] buffer;
		while (!tampon.empty())
		{
			i = tampon.find('\n');
			if (i == std::string::npos)
				break ;
			tampon2 = tampon.substr(0, i);
			tampon.erase(0, i + 1);
			if (tampon2.empty() || tampon2.find(' ') == std::string::npos)
			{
				if (!tampon2.empty())
					std::cerr << "config > \'" << var_pair.first << "\' : " << \
					"\'" << tampon2 << "\' syntax error in the file \'" << var_pair.second << \
					"\' (continued)" << std::endl;
				continue ;
			}
			std::string tmp = tampon2.substr(0, tampon2.find(' '));
			new_mime_tab[tmp] = tampon2.erase(0, tampon2.find(' ') + 1);
		}
		this->_mime_types = new_mime_tab;
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a path (ignored)" << std::endl;
}

std::string		MasterConfig::find_mime_type(std::string const &content, bool is_filename) const
{
	std::string											extension;
	std::map<std::string, std::string>::const_iterator	it;

	if (is_filename && content.find('.') != std::string::npos)
	{
		extension = content.substr(content.rfind('.'), content.size());
		it = this->_mime_types.find(extension);
		if (it != this->_mime_types.end())
			return ((*it).second);
	}
	if (!is_filename)
	{
		size_t i;
		for (i = 0 ; i < content.size() ; i++)
			if (!std::isprint(content[i]) && !std::isspace(content[i]))
				break ;
		if (i == content.size())
			return ("text/plain");
	}
	return (this->_default_mime);
}

void \
MasterConfig::set_upload_rights(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0]) \
	&& values[0].size() == 3)
	{
		std::istringstream(values[0]) >> this->_upload_rights;		// integer value (ex: 753)
		this->_upload_rights = permission_flags(_upload_rights);	// flags (ex: 111 110 011)
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive must be a 3-digits positive number (ignored)" << std::endl;
}

void \
MasterConfig::set_default_mime_type(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	if (values.size())
		this->_default_mime = var_pair.second;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a string value (ignored)" << std::endl;
}

void \
MasterConfig::set_timeout(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	(void)var_pair;
	if (values.size() == 1 && MasterConfig::is_there_only_digits(values[0], true))
	{
		double	raw_timeout;
		std::istringstream(values[0]) >> raw_timeout;
		this->_timeout = static_cast<int>(raw_timeout * 1000);
	}
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a positive (decimal) number (ignored)" << std::endl;
}

int	\
MasterConfig::permission_flags(int rights)
{
	int	const arr_rights[3]	= {
		rights / 100,			// user
		(rights % 100) / 10,	// group
		rights % 10				// other
	};
	int rflags			= 0;
	int	offset			= 6;

	for (int i = 0; i < 3; i++)
	{
		// read
		if (arr_rights[i] & (1 << 2))
			rflags = rflags | (1 << (offset + 2));
		
		// write
		if (arr_rights[i] & (1 << 1))
			rflags = rflags | (1 << (offset + 1));
		
		// exec
		if (arr_rights[i] & 1)
			rflags = rflags | (1 << offset);		

		offset -= 3;
	}
	return (rflags);
}
