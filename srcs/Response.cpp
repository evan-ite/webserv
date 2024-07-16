#include "../includes/server.hpp"

// Constructors
Response::Response() {}

Response::Response(int	status,
		std::string	reason,
		std::string	type,
		std::string	connection,
		std::string	body) 
{
	this->_status = status;
	this->_reason = reason;
	this->_type = type;
	this->_len = body.size();
	this->_date = getDateTime();
	this->_connection = connection;
	this->_body = body;
}

/*
location /: This configuration matches any request path that starts with /. It effectively serves as a catch-all for all requests that don't match more specific location blocks.

index index.html: Specifies that if the URL path ends with /, the server should look for index.html in the directory specified by root.

root content: Sets the root directory from which files will be served. In this case, files will be served from the content directory relative to Nginx's configured root.

error_page 404 /404.html: Defines the error page to display when a resource is not found (404 error).
*/
Response::Response(std::string const &httpRequest, Server serverData)
{
	Request request(httpRequest);
	Location loc = findLoc(request, serverData);
	std::string index = loc.index;
	std::string root = loc.root;

	Cgi cgi(request, serverData);

	try {
		if (cgi.isTrue()) 
			cgi.execute(*this);
		else if (request._method == POST)
			postMethod(request, serverData);
		else if (request._method == GET)
			getMethod(request, serverData, root, index);
		log(logDEBUG) << "Response object succesfully created";
	}
	catch (std::exception &e) {
        // Handle other methods or send a 405 Method Not Allowed response
        this->_status = 405;
        this->_reason = "Method Not Allowed";
        this->_type = "text/plain";
        this->_body = readFileToString("content/error/405.html");
        this->_connection = "close";
        this->_len = _body.length();
        this->_date = getDateTime();
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

void	Response::postMethod(Request request, Server serverData)
{

	(void) serverData;
	
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

void	Response::getMethod(Request request, Server serverData, std::string root, std::string index)
{
	(void) serverData;

	std::string file = root + request._location;
	// log(logDEBUG) << " " << file;
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
		this->_body = readFileToString("content/error/404.html");;
		this->_connection = "close";
		this->_len = 0;
	}
}


void	Response::deleteMethod() {}


/* Loops over all possible server locations and checks if they match the request location.
If no match was found, the first location in the map is used as default. */
Location	Response::findLoc(Request request, Server serverData) {
	Location	loc;
	bool		match = false;

	std::map<std::string, Location>::iterator it;
    for (it = serverData.locations.begin(); it != serverData.locations.end(); ++it) {
		std::size_t i = request._location.find(it->first);
		if (i == 0) {
			match = true;
			loc = it->second;
		}
	}
	if (!match)
		loc = serverData.locations.begin()->second; // Possible problem cause map doesn't preserve order of initiaization
	return loc;
}

void	Response::setStatus(int status) {
	this->_status = status;
	this->_date = getDateTime();
}

void	Response::setReason(std::string reason) {
	this->_reason = reason;
}

void	Response::setType(std::string type) {
	this->_type = type;
}

void	Response::setBody(std::string body) {
	this->_body = body;
	this->_len = body.size();
}

void	Response::setConnection(std::string connection) {
	this->_connection = connection;
}