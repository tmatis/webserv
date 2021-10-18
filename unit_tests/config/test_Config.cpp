# include "../car.hpp"

car_test test_parseConfig(void)
{
	MasterConfig mconfig;

	mconfig.construct("unit_tests/config/webserv.cnf");

	car_assert_cmp(mconfig._max_simultaneous_clients, 666);

	car_assert_cmp(mconfig._index_paths.size(), static_cast<unsigned long>(1));
	car_assert_cmp(mconfig._index_paths.find("index.html") \
	!= mconfig._index_paths.end(), true);

	car_assert_cmp(mconfig._root, "./www/");
	car_assert_cmp(mconfig._autoindex, true);

	car_assert_cmp(mconfig._configs.size(), static_cast<unsigned long>(2));

	car_assert_cmp(mconfig._configs[0].address, "127.0.0.1");
	car_assert_cmp(mconfig._configs[0].port_str, "659");
	car_assert_cmp(mconfig._configs[0].port, htons(659));
	car_assert_cmp(mconfig._configs[0].methods.size(), static_cast<unsigned long>(1));
	car_assert_cmp(mconfig._configs[0].server_names.find("localhost") \
	!= mconfig._index_paths.end(), true);

	car_assert_cmp(mconfig._configs[1].address, "0.0.0.0");
	car_assert_cmp(mconfig._configs[1].port_str, "8080");
	car_assert_cmp(mconfig._configs[1].port, htons(8080));
	car_assert_cmp(mconfig._configs[1].methods.size(), static_cast<unsigned long>(3));
	car_assert_cmp(mconfig._configs[1].server_names.size(), static_cast<unsigned long>(0));
	car_assert_cmp(mconfig._configs[1]._autoindex, false);

	// meant to be changed when default errors'll come
	car_assert_cmp(mconfig._configs[1]._error_pages.size(), static_cast<unsigned long>(0));

	car_assert_cmp(mconfig._configs[1]._root, "./server8080/");
	car_assert_cmp(mconfig._configs[1].body_limit, static_cast<unsigned long>(650));

	// switch from 2 to 3 since default route "/" was added
	car_assert_cmp(mconfig._configs[1].routes.size(), static_cast<unsigned long>(3));
	car_assert_cmp(mconfig._configs[1].routes[0].cgi_extension, ".php");
	car_assert_cmp(mconfig._configs[1].routes[0].cgi_path, "path");
	car_assert_cmp(mconfig._configs[1].routes[0].upload_path, "path");
	car_assert_cmp(mconfig._configs[1].routes[0].redirection.first, 0);
	car_assert_cmp(mconfig._configs[1].routes[0]._uploadfiles, false);

	car_assert_cmp(mconfig._configs[1].routes[1].cgi_extension, "");
	car_assert_cmp(mconfig._configs[1].routes[1]._uploadfiles, true);
	car_assert_cmp(mconfig._configs[1].routes[1].upload_path, "res/images/");
}
