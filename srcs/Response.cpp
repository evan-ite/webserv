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
	this->_date = getDateTime();
	this->_connection = connection;
	this->_body = body;
	this->_loc = NULL;
	this->_servSet = NULL;
}

Response::Response(Request &request, ServerSettings &serverData)
{
	this->_loc = new Location;
	*this->_loc = findLoc(request.getLoc(), serverData);
	this->_servSet = &serverData;

	Cgi cgi(&request, &serverData);

	try {
		if (cgi.isTrue())
			cgi.execute(*this);
		else if (request.getMethod() == POST && this->checkMethod("POST"))
			postMethod(request);
		else if (request.getMethod() == GET && this->checkMethod("GET"))
			getMethod(request);
		else if (request.getMethod() == DELETE && this->checkMethod("DELETE"))
			deleteMethod(request);
		else
			throw ResponseException("405");
		log(logDEBUG) << "Response object succesfully created";
	}
	catch (std::exception &e) {
		// Handle other methods or send a 405 Method Not Allowed response
		this->_status = atoi(e.what());
		this->_reason = getStatusMessage(e.what()); // function to get matchin reason for err code
		this->_type = "text/html";
		this->_body = readFileToString(findError(e.what()));
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
Response::~Response() {
	if (this->_loc)
		delete _loc;
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
	this->_loc = assign._loc;
	this->_servSet = assign._servSet;
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
			this->_reason = getStatusMessage("201");
			this->_type = "text/html";
			this->_connection = request.getConnection();
			this->_date = getDateTime();
			this->_body = readFileToString("content/upload_success.html");
			this->_len = _body.length();
			break;
		case 500:
			throw ResponseException("500");
		case 400:
			throw ResponseException("400");
	}
}

void Response::createFiles(Request &request, int &status) {
	std::string file = UPLOAD_DIR;
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
		std::string fullpath = file + filename;
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

void	Response::getMethod(Request &request)
{
	std::string file = _loc->root + request.getLoc();

	if (request.getLoc() == "/") // If no path specified
		file = _loc->root + "/" + _loc->index;
	else if (_loc->autoindex) { // Directory listing
		file = _loc->root + "/" + _loc->index;
		this->createDirlisting(file, _loc->path);
	}

	this->_status = 200;
	this->_body = readFileToString(file);
	this->_len = _body.length();
	this->_reason = getStatusMessage("200");
	this->_type = findType(file);
	this->_connection = "keep-alive";
	this->_date = getDateTime();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		throw ResponseException("404");
	}
}


void	Response::deleteMethod(Request &request) {
	std::string file = request.getLoc();
	if (!file.empty() && file[0] == '/')
		file = file.substr(1);

	if (remove(file.c_str()) != 0)
		throw ResponseException("404");
	else {
		this->_status = 200;
		this->_reason = getStatusMessage("200");
		this->_type = "text/html";
		this->_connection = "keep-alive";
		this->_date = getDateTime();
		this->_body = readFileToString("content/delete_success.html");
		this->_len = _body.length();
	}
}

/* Loops over all possible server locations and checks if they match the request location.
If no match was found, the first location in the map is used as default. */
Location Response::findLoc(const std::string& uri, ServerSettings sett)
{
	std::map<std::string, Location>::const_iterator it = (sett.locations).begin();
	for (; it != sett.locations.end(); ++it)
	{
		if (it->first == uri)
		{
			Location loc  = it->second;
			return (loc);
		}
	}

	size_t lastSlash = uri.find_last_of('/');
	if (lastSlash == 0)
		return (sett.locations["/"]);
	else if (lastSlash != std::string::npos)
	{
		std::string shortUri = uri.substr(0, lastSlash);
		return (this->findLoc(shortUri, sett));
	}
	else
		return sett.locations.at(0); // Handle the case when no match is found - we need a smarter way of just returning item 0?
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

// Check if the method is allowed in the location, argument should
// be method in capital letters, return value is true if method is allowed
bool	Response::checkMethod(std::string method) {
	Location loc = *this->_loc;
	std::vector<std::string>::iterator it = std::find(loc.allow.begin(), loc.allow.end(), method);

	if (it != loc.allow.end()) {
		return true; }
	return false;
}

// Creates an html page with name fileName that lists the content of dirPath
void	Response::createDirlisting(std::string fileName, std::string dirPath) {
	std::ofstream htmlFile(fileName.c_str());
	if (!htmlFile.is_open()) {
		log(logERROR) << "Error directory listing failed to open: " << fileName;
		throw ResponseException("500");
	}

	htmlFile << "<!DOCTYPE html>";
	htmlFile << "\n<html lang=\"en\">"; 
	htmlFile << "\n\t<head>\n\t\t<meta charset=\"UTF-8\">";
	htmlFile << "\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	htmlFile << "\n\t\t<link rel=\"stylesheet\" href=\"../styles.css\">";
	htmlFile << "\n\t\t<title>Directory " << dirPath << "</title>\n\t</head>";
	htmlFile << "\n\t<body>\n\t\t<h1>Directory " << dirPath << "</h1>";
	htmlFile << "\n\t\t<div class=\"formbox\">";
	htmlFile << this->loopDir(dirPath);	
	htmlFile << "\n\t\t</div>";
	htmlFile << "\n\t</body>\n</html>";
}

// Function that loops through directory and subdirectories and 
// creates html list of the content
std::string	Response::loopDir(std::string dirPath) {
	std::ostringstream html;

	if (dirPath[0] == '/' || dirPath[0] == '.') // Check if path starts with / or .
		dirPath = dirPath.substr(1);

	struct dirent	*entry;
	DIR		*dir = opendir(dirPath.c_str());
	if (dir == NULL) {
		log(logERROR) << "Error opening directory: " << dirPath;
		throw ResponseException("500");
	}

	// Loop through directory and create a list in html
	html << "\n\t\t<ul>";
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			html << "\n\t\t\t<li> " << entry->d_name << "</li>";
			if (entry->d_type == DT_DIR) {
				std::string	subPath = dirPath + "/" + entry->d_name;
				html << this->loopDir(subPath);
			}
		}
	}
	html << "\n\t\t</ul>";

	if (closedir(dir) != 0) {
		log(logERROR) << "Error closing directory: " << dirPath;
		throw ResponseException("500");
	}

	return html.str();
}

// Returns path to correct error page
std::string Response::findError(std::string errorCode) {
	if (_loc->loc_error_pages.find(errorCode) != _loc->loc_error_pages.end())
		return _loc->loc_error_pages[errorCode];
	else if (_servSet->error_pages.find(errorCode) != _servSet->error_pages.end())
		return _servSet->error_pages[errorCode];
	else
		return _servSet->error_pages["500"];
}

std::string Response::getStatusMessage(std::string statusCode) {
	if (_servSet->error_messages.find(statusCode) != _servSet->error_messages.end())
		return _servSet->error_messages[statusCode];
	else
		return "Internal Server Error";
}
