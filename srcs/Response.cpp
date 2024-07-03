#include "../includes/server.hpp"
#include "../includes/Response.hpp"


// Constructors
Response::Response(
	const std::string& status,
	const std::string& reason,
	const std::string& type,
	const std::string& connection,
	const std::string& body) :
	_status(status),
	_reason(reason),
	_type(type),
	_len(body.length()),
	_date(get_date_time()),
	_connection(connection),
	_body(body)
{}

/* Sets date and time to moment of copy */
Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_date(get_date_time()),
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
	_status = assign._status;
	_reason = assign._reason;
	_type = assign._type;
	_connection = assign._connection;
	_body = assign._body;
	_len = assign._len;
	_date = assign._date;
	return *this;
}

std::string Response::makeResponse()
{
	std::ostringstream response;

	response << HTTPVERSION << " " << _status << " " << _reason << "\r\n";
	response << "Date: " << _date << "\r\n";
	response << "Content-Length: " << _len << "\r\n";
	response << "Content-Type: " << _type << "\r\n";
	response << "Connection: " << _connection << "\r\n";
	response << "\r\n";
	response << _body;

	return response.str();
}


std::string get_date_time()
{
	std::time_t raw_time;
	std::time(&raw_time);
	struct std::tm *gmt_time = std::gmtime(&raw_time);
	char buffer[30];
	std::strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
	return std::string(buffer);
}
