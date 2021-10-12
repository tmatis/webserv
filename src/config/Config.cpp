/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:51:01 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 02:29:54 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Route::Route(void) :
	location("/"), root("/var/www"),
	autoindex(false), cgi_extension(""), upload_path("") {}

Config::Config(void) :
	name(""), body_limit(0) {}

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
