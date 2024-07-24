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

Response::Response(std::string const &httpRequest, Server serverData)
{
	Request request(httpRequest);
	Location loc = findLoc(request, serverData);
	std::string index = loc.index;
	std::string root = loc.root;

	Cgi cgi(&request, &serverData);

	try {
		if (cgi.isTrue())
			cgi.execute(*this);
		else if (request.getMethod() == POST)
			postMethod(request);
		else if (request.getMethod() == GET)
			getMethod(request, serverData, root, index);
		// else if delete
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

void	Response::postMethod(Request &request) {
	int status = 0;
	createFiles(request, status);
	switch (status) {
		case 201:
			this->_status = 201;
			this->_reason = "Created";
			this->_type = "text/html";
			this->_connection = request.getConnection();
			this->_date = getDateTime();
			this->_body = readFileToString("content/upload_success.html");
			this->_len = _body.length();
			break;
		case 500:
			this->_status = 500;
			this->_reason = "Internal Server Error";
			this->_type = "text/html";
			this->_connection = "close";
			this->_date = getDateTime();
			this->_body = readFileToString("content/error/500.html");
			this->_len = _body.length();
			break;
		case 400:
			this->_status = 400;
			this->_reason = "Bad Request";
			this->_type = "text/html";
			this->_connection = request.getConnection();
			this->_date = getDateTime();
			this->_body = readFileToString("content/error/400.html");
			this->_len = _body.length();
			break;
	}
}

void Response::createFiles(Request &request, int &status) {
	std::string path = UPLOAD_DIR;
	std::vector<std::pair<std::string, std::string> > fileData = request.getFileData();

	if (fileData.empty()) {
		log(logERROR) << "Bad request: no files to create";
		status = 400;
		return;
	}
	bool anyFailure = false;
	for (size_t i = 0; i < fileData.size(); ++i) {
		std::string filename = fileData[i].first;
		std::string content = fileData[i].second;
		std::string fullpath = path + filename;
		std::ofstream file(fullpath.c_str(), std::ios::binary);

		if (!file) {
			log(logERROR) << "Failed to open file for writing: " << fullpath;
			anyFailure = true;
			continue;
		}
		file.write(content.data(), content.size());
		if (!file.good()) {
			log(logERROR) << "Failed to write to file: " << fullpath;
			anyFailure = true;
			continue;
		}
		file.close();
	}
	if (anyFailure)
		status = 500;
	else
		status = 201;
}

void	Response::getMethod(Request request, Server serverData, std::string root, std::string index)
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
	this->_date = getDateTime();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		this->_status = 404;
		this->_reason = "not found";
		this->_type = "text/html";
		this->_body = readFileToString("content/error/404.html");
		this->_connection = "keep-alive";
		this->_len = this->_body.size();
	}
}


void	Response::deleteMethod(Request &request) {
	(void)request;
}






























/* Loops over all possible server locations and checks if they match the request location.
If no match was found, the first location in the map is used as default. */
Location	Response::findLoc(Request request, Server serverData) {
	Location	loc;
	bool		match = false;

	std::map<std::string, Location>::iterator it;
    for (it = serverData.locations.begin(); it != serverData.locations.end(); ++it) {
		std::size_t i = request.getLoc().find(it->first);
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
