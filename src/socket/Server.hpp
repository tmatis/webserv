/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/17 20:58:53 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>
# include <vector>
# include <unistd.h>
# include <sys/stat.h>
# include <dirent.h>

# include "Listener.hpp"
# include "Client.hpp"
# include "../config/Config.hpp"
# include "../http/HTTPURI.hpp"
# include "../config/Route.hpp"

class Server
{
	public:

		typedef std::vector<Client>::iterator		client_iterator;

		explicit Server(const Config& conf);
		virtual ~Server(void);

		// handle connections
		int		add_new_client(void);
		void	flush_clients(void);
		void	flush_files(void);

		// handle HTTP messages
		int		handle_request(Client& client);
		void	send_response(Client& client);
		int		create_file_response(Client& client);

		// getters
		std::vector<Client>&			get_clients(void);
		const std::vector<f_pollfd>&	get_files(void) const;
		const Listener&					get_listener(void) const;

	private:

		bool						_read_request(Client &client);
		const Route&				_resolve_routes(const std::string& uri_path);
		int							_check_request_validity(const Route& rules, HTTPRequest& request);
		int							_check_cgi_extension(const Route& rules, const std::string& uri_path);
		int							_find_resource(const Route& rules, std::string path, Client& client);
		bool						_is_index_file(const Route& rules, struct dirent* file);
		int							_handle_error(Client& client, int status, bool autogen = false);
		bool						_file_already_requested(Client& client, std::string const &filepath);
		std::string					_append_paths(const std::string& str1, const std::string& str2);
		void						_create_response(Client& client, const std::string *body = NULL);
		bool						_handle_redirection(Client& client, const Route& rules);
		std::string					_replace_conf_vars(Client& client, const std::string& redirection);
		bool						_handle_upload(Client& client, const Route& rules);

		Listener					_host;		// listener socket
		std::vector<Client>			_clients;	// list of clients connected
		std::vector<f_pollfd>		_files;		// files opened
		const Config&				_config;	// configuration of the server
};

#endif
