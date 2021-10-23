#include "HTTPResponse.hpp"
#include <sstream>
#include <iostream>

/* *********************** UTILITIES ************************* */

const short HTTPResponse::_status_code[35] =
	{
		100,
		101,
		102,
		103,
		200,
		201,
		202,
		204,
		301,
		302,
		303,
		304,
		307,
		400,
		401,
		403,
		404,
		405,
		406,
		408,
		409,
		410,
		411,
		412,
		413,
		414,
		415,
		416,
		417,
		500,
		501,
		502,
		503,
		504,
		505};

const std::string HTTPResponse::_status_message[35] =
	{
		"Continue",
		"Switching Protocols",
		"Processing",
		"Early Hints",
		"OK",
		"Created",
		"Accepted",
		"No Content",
		"Moved Permanently",
		"Found",
		"See Other",
		"Not Modified",
		"Temporary Redirect",
		"Bad Request",
		"Unauthorized",
		"Forbidden",
		"Not Found",
		"Method Not Allowed",
		"Not Acceptable",
		"Request Timeout",
		"Conflict",
		"Gone",
		"Length Required",
		"Precondition Failed",
		"Payload Too Large",
		"Request-URI Too Long",
		"Unsupported Media Type",
		"Requested Range Not Satisfiable",
		"Expectation Failed",
		"Internal Server Error",
		"Not Implemented",
		"Bad Gateway",
		"Service Unavailable",
		"Gateway Timeout",
		"HTTP Version Not Supported"};

std::string const &HTTPResponse::status_code_to_string(short code)
{
	for (size_t i = 0; i < sizeof(_status_code) / sizeof(_status_code[0]); i++)
	{
		if (_status_code[i] == code)
			return _status_message[i];
	}
	return _status_message[0];
}


// date at formatDate: <day-name>, <jour> <mois> <année> <heure>:<minute>:<seconde> GMT
// "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" or "Sun"
// day: 01 to 31
// month: 01 to 12
// year: 4 digits
// hour: 00 to 23
// minute: 00 to 59
// second: 00 to 59
// GMT: timezone
static std::string getDate(void)
{
	time_t t = time(NULL);
	struct tm *tmp = gmtime(&t);
	char buff[128];
	strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S GMT", tmp);
	return (std::string(buff));
}

template <typename T>
std::string itoa(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

/* ********************** CONSTRUCTORS *********************** */

HTTPResponse::HTTPResponse(void)
	: HTTPGeneral(), _status(OK), _is_ready(false),
		_header_parsed(false), _cgi_res_buffer()
{
}

HTTPResponse::HTTPResponse(HTTPResponse const &other)
	: HTTPGeneral(other), _status(other._status),
		_is_ready(other._is_ready),
		_header_parsed(other._header_parsed),
		_cgi_res_buffer(other._cgi_res_buffer)
{
}

HTTPResponse::~HTTPResponse(void)
{
}

/* ********************** OPERATORS ************************* */


HTTPResponse &HTTPResponse::operator=(HTTPResponse const &other)
{
	if (this != &other)
	{
		HTTPGeneral::operator=(other);
		_status = other._status;
		_is_ready = other._is_ready;
		_header_parsed = other._header_parsed;
		_cgi_res_buffer = other._cgi_res_buffer;
	}
	return *this;
}

/* ************************* GETTERS ************************* */

bool HTTPResponse::isReady(void) const
{
	return _is_ready;
}


short HTTPResponse::getStatus(void) const
{
	return _status;
}

/* ************************* SETTERS ************************* */

void HTTPResponse::setReady(bool b)
{
	_is_ready = b;
}

void HTTPResponse::setStatus(short code)
{
	_status = code;
}

void HTTPResponse::setContentType(std::string const &type)
{
	if (type.empty())
		return;
	// check if we need to add the charset
	if (type == "text/html"
		|| type == "text/plain"
		|| type == "text/css"
		|| type == "text/javascript"
		|| type == "text/xml"
		|| type == "application/json"
		|| type == "application/xml"
		|| type == "application/x-www-form-urlencoded")

		_header.setValue("Content-Type", type + "; charset=UTF-8");
	else
		_header.setValue("Content-Type", type);
}

void HTTPResponse::setConnection(HTTPConnectionType type)
{
	if (type == HTTP_CONNECTION_KEEP_ALIVE)
		_header.setValue("Connection", "keep-alive");
	else if (type == HTTP_CONNECTION_CLOSE)
		_header.setValue("Connection", "close");
}

void HTTPResponse::setAllow(std::string const &allow)
{
	_header.setValue("Allow", allow);
}

void HTTPResponse::setLocation(std::string const &location)
{
	_header.setValue("Location", location);
}

/* ************************* METHODS ************************* */

void HTTPResponse::clear(void)
{
	HTTPGeneral::clear();
	_status = OK;
	_is_ready = false;
	_header_parsed = false;
	_cgi_res_buffer.clear();
}

void HTTPResponse::_applyCGI(HTTPHeader &header)
{
	std::string const *status = header.getValue("Status");

	if (status)
		this->setStatus(static_cast<short>(atoi(status->c_str())));
	else
		_status = OK;

	this->setHeader(header);
	header.clear();
	_header_parsed = true;
}

void HTTPResponse::parseCGI(std::string const &str)
{
	static HTTPHeader header;

	_cgi_res_buffer += str;

	while (!_header_parsed && find_nl(_cgi_res_buffer).first != std::string::npos)
	{
		std::pair<size_t, short> pos = find_nl(_cgi_res_buffer);

		std::string line = _cgi_res_buffer.substr(0, pos.first);
		_cgi_res_buffer.erase(0, pos.first + pos.second);

		if (line.empty())
		{
			this->_applyCGI(header);
			break;
		}
		else if (!_header_parsed)
		{
			if (line.find(':') == std::string::npos)
			{
				this->_applyCGI(header);
				_body += line;
				if (pos.second == 2)
					_body += "\r\n";
				else
					_body += "\n";
			}
			else
				header.parseLine(line);
		}
	}
	if (_header_parsed)
	{
		_body += _cgi_res_buffer;
		_cgi_res_buffer.clear();
	}
}

// transform response to string ready to be sent

std::string HTTPResponse::toString(void)
{
	std::string res;

	_header.setValue("Server", "Webserv");
	_header.setValue("Date", getDate());
	_header.setValue("Content-Length", itoa(_body.size()));
	if (!_header.getValue("Content-Type"))
		setContentType("text/html");
	if (!_header.getValue("Connection"))
		setConnection(HTTP_CONNECTION_KEEP_ALIVE);
	res += "HTTP/1.1 " + itoa(_status) + " " + status_code_to_string(_status) + "\r\n";
	res += _header.toString();
	res += "\r\n";
	res += _body;
	return (res);
}

HTTPResponse	&HTTPResponse::gen_error_page(int const &status)
{
	unsigned int	i = 0;
	std::string		tmplate;
	std::string		error;

	tmplate += "<html>\n<head><title>{error}</title></head>\n<body>\n<center>";
	tmplate += "<h1>{error}</h1></center>\n<hr><center>webserv du feu <img src=\"h";
	tmplate += "ttps://i.imgur.com/z81h8VU.gif\"></center>\n</body>\n</html>\n";
	for ( ; i < sizeof(this->_status_code) / sizeof(this->_status_code[0]); i++)
		if (this->_status_code[i] == status)
			break ;
	if (i == sizeof(this->_status_code) / sizeof(this->_status_code[0]))
		error = "??? Unknown Error";
	else
	{
		error += itoa(status);
		error += " ";
		error += this->_status_message[i];
	}
	tmplate.replace(tmplate.find("{error}"), 7, error);
	tmplate.replace(tmplate.find("{error}"), 7, error);
	this->_body = tmplate;
	this->_status = static_cast<status_code>(status);
	return (*this);
}

HTTPResponse	&HTTPResponse::gen_autoindex(std::vector<struct dirent> const &files, \
std::string const &dir, std::string const &uri_path)
{
	unsigned int									i = 0;
	std::string										tmplate;
	std::string										new_entry;
	std::string										tampon;
	char											tbuffer[50];
	struct stat										file_stat;
	struct tm										*stime;
	std::set<std::string>							sdir;
	std::set<std::string>							sfiles;
	std::set<std::string>::iterator					it;

	tmplate += "<html>\n<head><title>Index of " + dir + "</title></head>\n";
	tmplate += "<body>\n<h1>Index of " + uri_path + "</h1>\n<hr>\n<pre>";
	tmplate += "<a href=\"../\">../</a>\n";

	while (i < files.size())
	{
		if (std::string(".") == files[i].d_name || std::string("..") == files[i].d_name)
		{
			i++;
			continue ;
		}
		if (files[i].d_type == DT_DIR)
			sdir.insert(files[i].d_name);
		else
			sfiles.insert(files[i].d_name);
		i++;
	}
	for (it = sdir.begin() ; it != sdir.end() ; ++it)
	{
		i = 1;
		memset(tbuffer, 0, 50);

		tampon = HTTPGeneral::append_paths(dir, *it);
		if (lstat(tampon.c_str(), &file_stat) == -1)
			i = 0;
		tampon = HTTPGeneral::append_paths(uri_path, *it) + '/';
		new_entry = "<a href=\"" + tampon + "\">";
		tampon = *it + "/";
		
		if (tampon.size() < 50)
			new_entry += std::string(tampon + "</a>").append(50 - tampon.size() + 1, ' ');
		else
			new_entry += tampon.substr(0, 47).append("..&gt;</a> ");
		if (i && (stime = localtime(&file_stat.st_mtime)) && \
		strftime(tbuffer, 49, "%d-%b-%Y %H:%M", stime))
			new_entry += std::string(tbuffer) + "                   -";
		else
			new_entry += "\?\?-\?\?\?-\?\?\?\? \?\?:\?\?                   -";
		tmplate += new_entry + "\n";
	}
	for (it = sfiles.begin() ; it != sfiles.end() ; ++it)
	{
		i = 1;
		memset(tbuffer, 0, 50);

		tampon = HTTPGeneral::append_paths(dir, *it);
		if (lstat(tampon.c_str(), &file_stat) == -1)
			i = 0;
		tampon = HTTPGeneral::append_paths(uri_path, *it);
		new_entry = "<a href=\"" + tampon + "\">";
		tampon = *it;

		if (tampon.size() < 50)
			new_entry += std::string(tampon + "</a>").append(50 - tampon.size() + 1, ' ');
		else
			new_entry += tampon.substr(0, 47).append("..&gt;</a> ");
		if (i && (stime = localtime(&file_stat.st_mtime)) && \
		strftime(tbuffer, 49, "%d-%b-%Y %H:%M", stime))
			new_entry += std::string(tbuffer);
		else
			new_entry += "\?\?-\?\?\?-\?\?\?\? \?\?:\?\?";
		if (i)
		{
			tampon = itoa(file_stat.st_size);
			new_entry.append(20 - tampon.size(), ' ').append(tampon);
		}
		else
			new_entry += "                   ?";
		tmplate += new_entry + "\n";
	}
	tmplate += "</pre><hr><center>webserv du feu <img src=\"h";
	tmplate += "ttps://i.imgur.com/z81h8VU.gif\"></center>\n</body>\n</html>\n";
	this->_body = tmplate;
	this->_status = static_cast<status_code>(200);
	return (*this);
}

HTTPResponse &HTTPResponse::gen_upload_response(const std::string& uri_path, \
		const std::vector<std::string>& files)
{
	_body.clear();
	_body += "<html>\n<head><title>Upload succesful !</title></head>\n";
	_body += "<body>\n<h1>File(s) uploaded succesfully</h1>";

	// add links to uploaded files on server
	std::vector<std::string>::const_iterator it = files.begin();
	while (it != files.end())
	{
		_body += "<a href=\"" + *it + "\">";
		_body += it->substr(it->rfind('/') + 1, std::string::npos);
		_body += "</a></br>\n";
		++it;
	}

	// link to previous page
	_body += "<hr><a href=\"" + uri_path + "\"> return to previous page </a>\n";
	
	// webserv signature
	_body += "<hr><center>webserv du feu <img src=\"h";
	_body += "ttps://i.imgur.com/z81h8VU.gif\"></center>\n</body>\n</html>\n";
	return (*this);
}
