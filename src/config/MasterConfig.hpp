/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 11:16:04 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/18 18:01:45 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTERCONFIG_HPP
# define MASTERCONFIG_HPP

# include <iostream>
# include <exception>
# include <string>
# include <set>
# include <map>
# include <vector>
# include <unistd.h>
# include <pwd.h>
# include <cctype>
# include <utility>
# include <stdexcept>
# include <fstream>
# include <sstream>

struct Config;

class MasterConfig
{
	public:
	// webserv proper config
	std::string								_server_name_version;
	unsigned long							_flags;
	bool									_autoindex;
	bool									_uploadfiles;
	int										_max_simultaneous_clients; // need to add a static var to server or client class
	std::string								_user; // setuid setgid
	std::ofstream							_error_log;
	std::streambuf							*_old_cerr;
	std::string								_root;

	// html proper config
	std::set<std::string>					_methods_supported;
	std::string								_default_mime;
	std::map<std::string, std::string>		_mime_types;
	std::set<std::string>					_index_paths;
	std::map<int, std::string>				_error_pages;
	unsigned int							_upload_rights;

	std::vector<Config>						_configs;

	std::pair<std::string, std::string>		extract_key_value(std::string &line);

	void	set_autoindex(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_uploadfiles(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_max_simultaneous_clients(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_user(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_error_log(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_root(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_index_paths(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_error_pages(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_mime_types(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_upload_rights(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);

	static void								keep_only_printable_chars(std::string &edit);
	static bool								is_there_only_digits(std::string const &edit);
	static void								remove_comments(std::string &edit);

	public:
	MasterConfig();
	MasterConfig(MasterConfig const &cp);
	~MasterConfig();

	MasterConfig			&operator=(MasterConfig const &rhs);
	void					construct(std::string const &config_path = "webserv.cnf");
	void					fill_var(std::pair<std::string, std::string> const &var_pair);
	void					server_construct(std::string &server_data);
	std::string				find_mime_type(std::string const &content, bool is_filename = true);


	class ErrorAtLine : public std::exception
	{
		ErrorAtLine(unsigned int const &line, std::string const &instruction) throw();
		virtual ~ErrorAtLine() throw();

		virtual char const *what() const throw();
	};
};

#endif
