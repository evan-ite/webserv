#include "../includes/server.hpp"

// Constructors
Response::Response() {}

Response::Response(std::string const &httpRequest, ConfigData confData)
{
	Request request(httpRequest);
	LocationConfig loc = confData.locations["/"];
	std::string index = loc.index;
	std::string root = loc.root;

	if (request._method == POST && request._location == "/upload")
		postMethod(request);
	else if (request._method == GET)
		getMethod(request, root, index);
	else
	{
        // Handle other methods or send a 405 Method Not Allowed response
        this->_status = 405;
        this->_reason = "Method Not Allowed";
        this->_type = "text/plain";
        this->_body = "Method Not Allowed";
        this->_connection = "close";
        this->_len = _body.length();
        this->_date = getDateTime();
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

void	Response::postMethod(Request request)
{
	// (void)request;
	// Create all files
	for (size_t i = 0; i < request._fileData.size(); ++i) {

		std::string filename = request._fileData[i].first;
		std::string content = request._fileData[i].second;

		std::cout << "Filename: " << filename << "\nContent: " << content << "\n";
	}


	// Process the POST data (e.g., save it, respond with a success message, etc.)
	this->_status = 200;
	this->_body = "Received POST data: ";
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = "text/plain";
	this->_connection = "close"; // Generally, you close the connection after handling POST
	this->_date = getDateTime();
}

void	Response::getMethod(Request request, std::string root, std::string index)
{
	std::string file = root + request._location;
	if (request._location == "/")
		file = root + "/" + index;
	this->_status = 200;
	this->_body = readFileToString(file);
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = findType(file);
	this->_connection = "keep-alive";
	this->_date = getDateTime();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		this->_status = 404;
		this->_reason = "not found";
		this->_type = "";
		this->_body = "";
		this->_connection = "close";
		this->_len = 0;
	}
}

void	Response::deleteMethod() {}
