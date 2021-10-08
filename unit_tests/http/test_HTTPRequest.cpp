/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HTTPRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:00 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 15:40:35 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../car.hpp"

car_test test_parseChunk()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	assert(req.getMethod() == "GET");
	assert(req.getURI() == "/");
	assert(req.getVersion() == "HTTP/1.1");
	assert((*req.getHeader().getValue("Host"))[0] == "localhost:8080");
	assert(req.getBody() == "");
	assert(req.isReady() == true);
}
