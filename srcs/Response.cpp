#include "../includes/settings.hpp"

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
	this->_date = getDateTimeStr();
	this->_connection = connection;
	this->_body = body;
}

Response::Response(std::string const &httpRequest, ServerSettings* serverData)
{
	Request request(httpRequest);
	Location loc = findLoc(request.getLoc(), serverData);
	std::string index = loc.index;
	std::string root = loc.root;

	Cgi cgi(&request, serverData);

	try {
		if (cgi.isTrue())
			cgi.execute(*this);
		else if (request.getMethod() == POST)
			postMethod(request, serverData);
		else if (request.getMethod() == GET)
			getMethod(request, serverData, root, index);
		log(logDEBUG) << "Response object succesfully created";
	}
	catch (std::exception &e) {
		// Handle other methods or send a 405 Method Not Allowed response
		this->_status = 405;
		this->_reason = "Method Not Allowed";
		this->_type = "text/html";
		this->_body = readFileToString("content/error/405.html");
		this->_connection = "keep-alive";
		this->_len = _body.length();
		this->_date = getDateTimeStr();
	}
}

/* Sets date and time to moment of copy */
Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_date(getDateTimeStr()),
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

void	Response::postMethod(Request request, ServerSettings* serverData)
{

	(void) serverData;

	// Create all files
	for (size_t i = 0; i < request.getFileData().size(); ++i) {

		std::string filename = request.getFileData()[i].first;
		std::string content = request.getFileData()[i].second;

		std::cout << "Filename: " << filename << "\nContent: " << content << "\n";
	}

	// Process the POST data (e.g., save it, respond with a success message, etc.)
	this->_status = 200;
	this->_body = "Received POST data: ";
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = "text/plain";
	this->_connection = "close"; // Generally, you close the connection after handling POST
	this->_date = getDateTimeStr();
}

void	Response::getMethod(Request request, ServerSettings* serverData, std::string root, std::string index)
{
	(void) serverData;

	std::string file = root + request.getLoc();
	// log(logDEBUG) << " " << file;
	if (request.getLoc() == "/")
		file = root + "/" + index;
	this->_status = 200;
	this->_body = readFileToString(file);
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = findType(file);
	this->_connection = "keep-alive";
	this->_date = getDateTimeStr();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		this->_status = 404;
		this->_reason = "not found";
		this->_type = "text/html";
		this->_body = readFileToString("content/error/404.html");;
		this->_connection = "keep-alive";
		this->_len = this->_body.size();
	}
}


void	Response::deleteMethod() {}


/* Loops over all possible server locations and checks if they match the request location.
If no match was found, the first location in the map is used as default. */
Location Response::findLoc(const std::string& uri, ServerSettings* sett)
{
	log(logDEBUG) << "1 WE ARE HERE " << uri;
	std::cout << *sett;
	std::map<std::string, Location>::const_iterator it = (sett->locations).begin();
	for (; it != sett->locations.end(); ++it)
	{
		log(logDEBUG) << "2 WE ARE HERE " << uri;
		log(logDEBUG) << it->second;
		log(logDEBUG) << "2 WE ARE HERE " << uri;

		if (it->first == uri)
		{
			Location loc  = it->second;
			return (loc);
		}
	}

	log(logDEBUG) << "3 WE ARE HERE " << uri;

	size_t lastSlash = uri.find_last_of('/');
	if (lastSlash == 0)
		return (sett->locations["/"]);
	else if (lastSlash != std::string::npos)
	{
		std::string shortUri = uri.substr(0, lastSlash);
		return (this->findLoc(shortUri, sett));
	}
	else
		return sett->locations.at(0); // Handle the case when no match is found - we need a smarter way of just returning item 0?
}



void	Response::setStatus(int status) {
	this->_status = status;
	this->_date = getDateTimeStr();
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
