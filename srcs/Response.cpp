#include "../includes/server.hpp"

// Constructors
Response::Response()
{}

Response::Response(std::string const &httpRequest, ConfigData const confData)
{
	Request request(httpRequest);

	// this will come from conf file soon
	std::string index = "index.html";
	std::string root = "html";
	(void)confData; // add parser

	std::string file = root + "/"+ request._location;
	if (request._location == "/")
		file = root + "/" + index;

	this->_status = 200;
	this->_body = readFileToString(file);
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = findType(file);
	this->_connection = "keep-alive";
	this->_date = getDateTime();

	if (this->_body == "" || this->_type == "") {
		this->_status = 404;
		this->_reason = "not found";
		this->_type = "";
		this->_body = "";
		this->_connection = "close";
		this->_len = 0;
	}

	log(logDEBUG) << "Response object succesfully created";
}

/* Sets date and time to moment of copy */
Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_date(getDateTime()),
	_connection(copy._connection),
	_body(copy._body)
{}

// Destructor
Response::~Response() {}

// Operators
Response & Response::operator=(const Response &assign)
{
	this->_status = assign._status;
	this->_reason = assign._reason;
	this->_type = assign._type;
	this->_connection = assign._connection;
	this->_body = assign._body;
	this->_len = assign._len;
	this->_date = assign._date;
	return (*this);
}

std::string Response::makeResponse()
{
	std::ostringstream response;

	response << HTTPVERSION << " " << this->_status << " " << this->_reason << "\r\n";
	response << "Date: " << this->_date << "\r\n";
	response << "Content-Length: " << this->_len << "\r\n";
	if (this->_type != "")
		response << "Content-Type: " << this->_type << "\r\n";
	response << "Connection: " << this->_connection << "\r\n";
	response << "\r\n";
	std::string return_value = response.str();
	if (this->_len)
		return_value += this->_body + "\r\n";
	return (return_value);
}


std::string getDateTime()
{
	std::time_t raw_time;
	std::time(&raw_time);
	struct std::tm *gmt_time = std::gmtime(&raw_time);
	char buffer[30];
	std::strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
	return std::string(buffer);
}
