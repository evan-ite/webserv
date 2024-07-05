#include "../includes/server.hpp"

Request::Request(std::string httpRequest)
{
	this->parse(httpRequest);
}

Request::Request() {}

Request::Request(const Request &copy)
{
	this->_method = copy._method;
	this->_host = copy._host;
	this->_contentLenght = copy._contentLenght;
	this->_location = copy._location;
	this->_userAgent = copy._userAgent;
	this->_connection = copy._connection;
}

Request::~Request() {}

Request & Request::operator=(const Request &assign)
{
	this->_method = assign._method;
	this->_host = assign._host;
	this->_contentLenght = assign._contentLenght;
	this->_location = assign._location;
	this->_userAgent = assign._userAgent;
	this->_connection = assign._connection;
	return *this;
}

void Request::parse(std::string httpRequest)
{
	std::string	method = splitReturnFirst(httpRequest, " ");
	if (method == "GET")
	{
		this->_method = GET;
		this->_location = findKey(httpRequest, "GET ", ' ');
	}
	else if (method == "POST")
	{
		this->_method = POST;
		this->_location = findKey(httpRequest, "POST ", ' ');
	}
	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = findKey(httpRequest, "DELETE ", ' ');
	}
	else
		throw Request::invalidMethod();
	this->_userAgent = findKey(httpRequest, "User-Agent:", '\n');
	this->_host = findKey(httpRequest, "Host:", '\n');
	this->_connection = findKey(httpRequest, "Connection:", '\n');
	this->_contentLenght = atoi(findKey(httpRequest, "Content-Length:", '\n').c_str());
	log(logDEBUG) << "Request object created:\n" << this->_method << "\n" << this->_location << "\n" << this->_userAgent << "\n" << this->_host << "\n" << this->_connection << "\n" << this->_contentLenght;
}
