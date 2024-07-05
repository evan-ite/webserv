#include "../includes/server.hpp"

// Constructors
Response::Response()
{}

Response::Response(std::string const &httpRequest, ConfigData const confData)
{
	Request request(httpRequest);

    std::string location = request._location;
    std::string path = confData.locations.at(location).path;
    std::string index = confData.locations.at(location).index;

    this->_status = 200;
    this->_body = readFileToString(path + index);
    this->_reason = "ok";
    this->_type = findType(index);
    this->_connection = "keep-alive";

    if (this->_body == "" || this->_type == "") {
        this->_status = 404;
        this->_reason = "not found";
        this->_connection = "close";
        this->_type = "";
        this->_body = "";
    }
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
Response::~Response()
{
}


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
	return *this;
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
	if (this->_body != "")
		response << this->_body;

	return response.str();
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
