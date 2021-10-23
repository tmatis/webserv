/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/23 21:52:29 by mamartin         ###   ########.fr       */
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

# define SERVER_TIMEOUT 15.0 // seconds

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
		int		write_uploaded_file(Client& client, int index);

		// getters
		std::vector<Client>&			get_clients(void);
		const std::vector<f_pollfd*>&	get_files(void) const;
		const Listener&					get_listener(void) const;

		static const int				timeout;

	private:
	
		typedef int (*method_handler)(Client&, const Route&, const HTTPURI&);

		/* GLOBAL USAGE ===================================================== */
		/*** REQUESTS *********************************************************/
		bool			_read_request(Client &client);
		const Route&	_resolve_routes(const std::string& uri_path);
		int				_check_request_validity(const Route& rules, HTTPRequest& request);
		bool			_is_mime_type_supported(const Route& rules, const std::string& mime_type);
		/*** RESPONSES ********************************************************/
		int				_handle_error(Client& client, int status, bool autogen = false);
		void			_create_response(Client& client);
		void			_define_content_type(Client& client, HTTPResponse& response);
		std::string		_get_uri_reference(const std::string& filename);

		/* METHOD HANDLERS ================================================== */
		int				_handle_get(Client &client, const Route& rules, const HTTPURI& uri);
		int				_handle_post(Client &client, const Route& rules, const HTTPURI& uri);
		int				_handle_delete(Client &client, const Route& rules, const HTTPURI& uri);
		/*** GET **************************************************************/
		int				_find_resource(const Route& rules, const std::string& uri_path, Client& client);
		bool			_is_index_file(const Route& rules, struct dirent* file);
		bool			_file_already_requested(Client& client, std::string const &filepath);
		/*** POST *************************************************************/
		bool			_handle_upload(Client& client, const Route& rules);
		void			_raw_upload(Client& client, const Route& rules, const std::string& mime);
		void			_form_upload(Client& client);
		std::string		_get_boundary(const std::string* content_type);
		std::string		_get_file_info(const std::string& body, std::string& type, size_t *start);
		std::string		_get_next_line(const std::string& src, size_t* pos);
		f_pollfd*		_create_file(const std::string& filename, const std::string& data, uint mode);
		void			_clear_previous_files(Client& client);
		int				_list_directory(std::vector<std::string>& files, const std::string& path);
		void			_check_existing_file(std::vector<std::string>& files, std::string& filename);

		/* OTHER ============================================================ */
		/*** CGI **************************************************************/
		int				_check_cgi_extension(const Route& rules, const std::string& uri_path);
		/*** REDIRECTIONS *****************************************************/
		bool			_handle_redirection(Client& client, const Route& rules);
		std::string		_replace_conf_vars(Client& client, const std::string& redirection);
		
		Listener				_host;		// listener socket
		std::vector<Client>		_clients;	// list of clients connected
		std::vector<f_pollfd*>	_files;		// files opened
		const Config&			_config;	// configuration of the server
};

#endif
