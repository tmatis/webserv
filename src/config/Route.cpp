/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 21:48:00 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/18 03:19:40 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Route.hpp"
# include "Config.hpp"

Route::Route(Config const &base) : Config(base)
{
	this->location = "/";
	this->upload_path = ".";
	this->cgi_extension = std::string();
}
Route::Route(Route const &cp) { *this = cp; }
Route::~Route() {}

Route		&Route::operator=(Route const &rhs)
{
	this->Config::operator=(rhs);
	this->location = rhs.location;
	this->upload_path = rhs.upload_path;
	this->cgi_extension = rhs.cgi_extension;
	this->cgi_path = rhs.cgi_path;
	return (*this);
}

void		Route::construct(std::pair<std::string, std::string> &config_str)
{
	std::vector<std::pair<std::string, std::string> >	raw_data;
	std::pair<std::string, std::string>					parsing_res;

	try {
		this->location = config_str.first.substr(1);
		parsing_res = this->extract_key_value(config_str.second);
		while (!parsing_res.first.empty())
		{
			raw_data.push_back(parsing_res);
			parsing_res = this->extract_key_value(config_str.second);
		}
		unsigned int i = 0;
		while (i < raw_data.size())
		{
			MasterConfig::keep_only_printable_chars(raw_data[i].first);
			MasterConfig::keep_only_printable_chars(raw_data[i].second);
			this->fill_var(raw_data[i]);
			i++;
		}
	} catch (std::exception &e) { throw ; }
}

void		Route::fill_var(std::pair<std::string, std::string> const &var_pair)
{
	unsigned long					i = 0;
	std::vector<std::string>		values;
	std::string						values_raw = var_pair.second;

	std::string	const str_args[11] = {"upload_rights", "index", "error_page", "root", \
	"autoindex", "redirection", "body_limit", "upload_files", "methods", "cgi", \
	"upload_path"};

	typedef void (Route::*func_setter)
					(std::pair<std::string, std::string> const &var_pair,
					std::vector<std::string> const &values);
					
	func_setter const func_args[11] = {&Route::set_upload_rights, &Route::set_index_paths, \
	&Route::set_error_pages, &Route::set_root, &Route::set_autoindex, \
	&Route::set_redirection, &Route::set_body_limit, &Route::set_uploadfiles, \
	&Route::set_methods, &Route::set_cgi, &Route::set_upload_path};

	while (!values_raw.empty())
	{
		i = values_raw.find(' ');
		if (i == std::string::npos)
			i = values_raw.size();
		values.push_back(values_raw.substr(0, i));
		values_raw.erase(0, i + 1);
	}
	for (i = 0 ; i < 11 ; i++)
	{
		if (var_pair.first == str_args[i])
		{
			(this->*func_args[i])(var_pair, values);
			return ;
		}
	}
	std::cerr << "config > \'" << var_pair.first << "\' : this " << \
	"argument doesn't exist in the route scope (ignored)" << std::endl;
}

void \
Route::set_cgi(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	std::string						extension;
	std::string						path = var_pair.second;

	if (values.size() >= 2)
	{
		extension = values[0];
		path.erase(0, extension.size() + 1);
		if (extension[0] != '.')
			extension.insert(0, ".");
		this->cgi_extension = extension;
		this->cgi_path = path;
	}
	else
	{
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs an extension and a path (fatal error)" << std::endl;
		throw std::invalid_argument("cgi block must be well filled");
	}
}

void \
Route::set_upload_path(std::pair<std::string, std::string> const &var_pair, \
std::vector<std::string> const &values)
{
	if (values.size())
		this->upload_path = var_pair.second;
	else
		std::cerr << "config > \'" << var_pair.first << "\' : this " << \
		"directive needs a path (ignored)" << std::endl;
}
