# include "../car.hpp"

car_test test_parseConfig(void)
{
	MasterConfig mconfig;

	mconfig.construct("unit_tests/config/webserv.cnf");

	assert(mconfig._max_simultaneous_clients == 666);

	assert(mconfig._index_paths.size() == 1);
	assert(mconfig._index_paths.find("index.html") \
	!= mconfig._index_paths.end());

	assert(mconfig._root == "./www/");
	assert(mconfig._autoindex);

	assert(mconfig._configs.size() == 2);

	assert(mconfig._configs[0].address == "127.0.0.1");
	assert(mconfig._configs[0].port == htons(80));
	assert(mconfig._configs[0].methods.size() == 1);
	assert(mconfig._configs[0].server_names.find("localhost") \
	!= mconfig._index_paths.end());

	assert(mconfig._configs[1].address == "0.0.0.0");
	assert(mconfig._configs[1].port == htons(8080));
	assert(mconfig._configs[1].methods.size() == 3);
	assert(!mconfig._configs[1].server_names.size());
	assert(!mconfig._configs[1]._autoindex);

	// meant to be changed when default errors'll come
	assert(!mconfig._configs[1]._error_pages.size());

	assert(mconfig._configs[1]._root == "./server8080/");
	assert(mconfig._configs[1].body_limit == 650);

	assert(mconfig._configs[1].routes.size() == 2);
	assert(mconfig._configs[1].routes[0].cgi_extension == ".php");
	assert(mconfig._configs[1].routes[0].cgi_path == "path");
	assert(mconfig._configs[1].routes[0].upload_path == "path");
	assert(mconfig._configs[1].routes[0].redirection.first == 0);
	assert(!mconfig._configs[1].routes[0]._uploadfiles);

	assert(mconfig._configs[1].routes[1].cgi_extension == "");
	assert(mconfig._configs[1].routes[1]._uploadfiles);
	assert(mconfig._configs[1].routes[1].upload_path == "res/images/");
}
