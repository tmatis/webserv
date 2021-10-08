/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 14:27:06 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 15:11:45 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "HTTPGeneral.hpp"

class HTTPRequest : public HTTPGeneral
{
	private:
		std::string _method;
		std::string _version;
		std::string _uri;
		bool		_is_ready;
		std::string _buffer;

	public:
		HTTPRequest(void);
		HTTPRequest(const HTTPRequest &src);
		HTTPRequest &operator=(const HTTPRequest &src);
		~HTTPRequest(void);

		void setMethod(std::string const &method);
		std::string const &getMethod(void) const;

		void setVersion(std::string const &version);
		std::string const &getVersion(void) const;

		void setURI(std::string const &uri);
		std::string const &getURI(void) const;

		bool isReady(void) const;

		void parseChunk(std::string const &chunk);

};
		

#endif
