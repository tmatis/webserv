/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_HTTPRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:00 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/16 14:45:36 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../car.hpp"

car_test test_parseChunk_basic_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");

	car_assert_cmp(req.getMethod(), "GET");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp((*req.getHeader().getValue("Host")), "localhost:8080");
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);
}

car_test test_parseChunk_multiple_chunk_get()
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1");
	req.parseChunk("\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n\r\n");
	car_assert_cmp(req.getMethod(), "GET");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp((*req.getHeader().getValue("Host")), "localhost:8080");
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);
}

car_test test_parseChunk_post_empty_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 0\r\n\r\n");

	car_assert_cmp(req.getMethod(), "POST");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp((*req.getHeader().getValue("Host")), "localhost:8080");
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);
}

car_test test_parseChunk_post_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 20\r\n\r\n");
	req.parseChunk("Hello World ");
	req.parseChunk("!!!!!!!!");

	car_assert_cmp(req.getMethod(), "POST");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp((*req.getHeader().getValue("Host")), "localhost:8080");
	car_assert_cmp(req.getBody(), "Hello World !!!!!!!!");
	car_assert_cmp(req.isReady(), true);
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

	car_assert_cmp(req.getMethod(), "GET");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp(req.getHost(), "localhost:8080");
	car_assert_cmp(*req.getUserAgent(), "Mozilla/5.0");
	car_assert_cmp(req.getAccept()[0], "text/html");
	car_assert_cmp(req.getAccept()[1], "text/plain");
	car_assert_cmp(req.getConnection(), HTTP_CONNECTION_KEEP_ALIVE);
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);
}

car_test multiple_request_per_chunk(void)
{
	HTTPRequest req;

	req.parseChunk("GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\n");
	car_assert(req.isReady() == false);
	req.parseChunk("Accept: text/html,text/plain\r\nConnection: keep-alive\r\n\r\nGET / HTTP/1.1\r\nHost: localhast:8080\r\nUser-Agent: Mozillo/5.0\r\nAccept: text/html,text/plain\r\nConnection: close\r\n\r\n");

	car_assert(req.isChunked() == false);

	car_assert_cmp(req.getMethod(), "GET");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp(req.getHost(), "localhost:8080");
	car_assert_cmp(*req.getUserAgent(), "Mozilla/5.0");
	car_assert_cmp(req.getAccept()[0], "text/html");
	car_assert_cmp(req.getAccept()[1], "text/plain");
	car_assert_cmp(req.getConnection(), HTTP_CONNECTION_KEEP_ALIVE);
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);

	req.clear();

	car_assert_cmp(req.getMethod(), "GET");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp(req.getHost(), "localhast:8080");
	car_assert_cmp(*req.getUserAgent(), "Mozillo/5.0");
	car_assert_cmp(req.getAccept()[0], "text/html");
	car_assert_cmp(req.getAccept()[1], "text/plain");
	car_assert_cmp(req.getConnection(), HTTP_CONNECTION_CLOSE);
	car_assert_cmp(req.getBody(), "");
	car_assert_cmp(req.isReady(), true);
}

car_test line_return_in_body(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html,text/plain\r\nConnection: keep-alive\r\nContent-Length: 14\r\n\r\nHello\r\nWorld\r\n");

	car_assert_cmp(req.getMethod(), "POST");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp(req.getHost(), "localhost:8080");
	car_assert_cmp(*req.getUserAgent(), "Mozilla/5.0");
	car_assert_cmp(req.getAccept()[0], "text/html");
	car_assert_cmp(req.getAccept()[1], "text/plain");
	car_assert_cmp(req.getConnection(), HTTP_CONNECTION_KEEP_ALIVE);
	car_assert_cmp(req.getBody(), "Hello\r\nWorld\r\n");
	car_assert_cmp(req.isReady(), true);
}

car_test chunked_request(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\n");

	car_assert(req.isReady() == false);
	car_assert_cmp(req.getMethod(), "POST");
	car_assert_cmp(req.getURI().getPath(), "/");
	car_assert_cmp(req.getVersion(), "HTTP/1.1");
	car_assert_cmp(req.getHost(), "localhost");
	car_assert_cmp(*req.getContentType(), "text/plain");

	req.parseChunk("Transfer-Encoding: chunked\r\n\r\n");

	car_assert(req.isReady() == false);
	car_assert(req.isChunked() == true);

	req.parseChunk("5\r\nHello\r\n");
	car_assert(req.isChunked() == true);
	car_assert(req.isReady() == false);
	req.parseChunk("6\r\n World\r\n");
	car_assert(req.isReady() == false);
	req.parseChunk("0\r\n\r\n");
	car_assert(req.isReady() == true);
	car_assert_cmp(req.getBody(), "Hello World");
}

car_test chunked_request_weird_format(void)
{
	HTTPRequest req;

	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\n");
	req.parseChunk("Transfer-Encoding: chunked\r\n\r\n");
	req.parseChunk("5");
	req.parseChunk("\r\nHello");
	req.parseChunk("\r\n");
	req.parseChunk("6");
	req.parseChunk("\r\n World");
	req.parseChunk("\r\n");
	req.parseChunk("0");
	req.parseChunk("\r\n\r\n");

	car_assert_cmp(req.getBody(), "Hello World");
	car_assert(req.isReady() == true);
}

car_test chunked_request_with_nl_body(void)
{
	HTTPRequest req;
	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\n");
	req.parseChunk("Transfer-Encoding: chunked\r\n\r\n");
	req.parseChunk("0C");
	req.parseChunk("\r\nHello\r\nWorld\r\n");
	req.parseChunk("0\r\n\r\n");

	car_assert_cmp(req.getBody(), "Hello\r\nWorld");
	car_assert(req.isReady() == true);
}

car_test chunked_request_with_nl_body_weird(void)
{
	HTTPRequest req;
	req.parseChunk("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\n");
	req.parseChunk("Transfer-Encoding: chunked\r\n\r\n");
	req.parseChunk("0C");
	req.parseChunk("\r\nHello\r\n");
	req.parseChunk("World\r\n");
	req.parseChunk("0\r\n\r\n");

	car_assert_cmp(req.getBody(), "Hello\r\nWorld");
	car_assert(req.isReady() == true);
}

