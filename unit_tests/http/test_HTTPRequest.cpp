/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HTTPRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:00 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/11 19:38:59 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../car.hpp"

car_test test_parseChunk_basic_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");

	assert(req.getMethod() == "GET");
	assert(req.getURI() == "/");
	assert(req.getVersion() == "HTTP/1.1");
	assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	assert(req.getBody() == "");
	assert(req.isReady() == true);
}

car_test test_parseChunk_multiple_chunk_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1");
	req.parseChunk("\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");
	assert(req.getMethod() == "GET");
	assert(req.getURI() == "/");
	assert(req.getVersion() == "HTTP/1.1");
	assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	assert(req.getBody() == "");
	assert(req.isReady() == true);
}

car_test test_parseChunk_post_empty_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 0\r\n\r\n");

	assert(req.getMethod() == "POST");
	assert(req.getURI() == "/");
	assert(req.getVersion() == "HTTP/1.1");
	assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	assert(req.getBody() == "");
	assert(req.isReady() == true);
}

car_test test_parseChunk_post_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 4\r\n\r\nbody");

	assert(req.getMethod() == "POST");
	assert(req.getURI() == "/");
	assert(req.getVersion() == "HTTP/1.1");
	assert((*req.getHeader().getValue("Host")) ==  "localhost:8080");
	assert(req.getBody() == "body");
	assert(req.isReady() == true);
}

