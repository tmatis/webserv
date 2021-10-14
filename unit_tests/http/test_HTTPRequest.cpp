/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HTTPRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:00 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/14 14:46:59 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../car.hpp"

car_test test_parseChunk_basic_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");

	car_assert(req.getMethod() == "GET");
	car_assert(req.getURI().getPath() == "/");
	car_assert(req.getVersion() == "HTTP/1.1");
	car_assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	car_assert(req.getBody() == "");
	car_assert(req.isReady() == true);
}

car_test test_parseChunk_multiple_chunk_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1");
	req.parseChunk("\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");
	car_assert(req.getMethod() == "GET");
	car_assert(req.getURI().getPath() == "/");
	car_assert(req.getVersion() == "HTTP/1.1");
	car_assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	car_assert(req.getBody() == "");
	car_assert(req.isReady() == true);
}

car_test test_parseChunk_post_empty_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 0\r\n\r\n");

	car_assert(req.getMethod() == "POST");
	car_assert(req.getURI().getPath() == "/");
	car_assert(req.getVersion() == "HTTP/1.1");
	car_assert((*req.getHeader().getValue("Host")) == "localhost:8080");
	car_assert(req.getBody() == "");
	car_assert(req.isReady() == true);
}

car_test test_parseChunk_post_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 20\r\n\r\n");
	req.parseChunk("Hello World ");
	req.parseChunk("!!!!!!!!");

	car_assert(req.getMethod() == "POST");
	car_assert(req.getURI().getPath() == "/");
	car_assert(req.getVersion() == "HTTP/1.1");
	car_assert((*req.getHeader().getValue("Host")) ==  "localhost:8080");
	car_assert(req.getBody() == "Hello World !!!!!!!!");
	car_assert(req.isReady() == true);
}

car_test test_parseChunk_post_body_with_multiple_chunk(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 4\r\n\r\nbody");
}
	

car_test test_parsed_get(void)
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html,text/plain\r\nConnection: keep-alive\r\n\r\n");
	
	car_assert(req.getMethod() == "GET");
	car_assert(req.getURI().getPath() == "/");
	car_assert(req.getVersion() == "HTTP/1.1");
	car_assert(req.getHost() == "localhost:8080");
	car_assert(*req.getUserAgent() == "Mozilla/5.0");
	car_assert(req.getAccept()[0] == "text/html");
	car_assert(req.getAccept()[1] == "text/plain");
	car_assert(req.getConnection() == HTTP_CONNECTION_KEEP_ALIVE);
	car_assert(req.getBody() == "");
	car_assert(req.isReady() == true);
}
