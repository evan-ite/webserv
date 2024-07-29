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
	this->_connection = connection;
	this->_body = body;
	this->_loc = NULL;
	this->_servSet = NULL;
}

Response::Response(Request &request, ServerSettings &serverData)
{
	try
	{
		this->_loc = new Location;
		*this->_loc = findLoc(request.getLoc(), serverData);
		if (!this->_loc->redir.empty()) 
		{
			// Handle external and internal redirection
			if (this->checkExternal())
				return ;
			std::string newLoc = removeSubstr(request.getLoc(), this->_loc->path);
			request.setLoc(newLoc);
			*this->_loc = findLoc(this->_loc->redir, serverData);
		}
		this->_servSet = &serverData;
		Cgi cgi(&request, &serverData);
		HttpMethod method = request.getMethod();

		if (!isValidRequest(request))
			throw ResponseException("400");
		else if (cgi.isTrue())
			cgi.execute(*this);
		else if (method == POST && this->checkMethod("POST"))
			postMethod(request);
		else if (method == GET && this->checkMethod("GET"))
			getMethod(request);
		else if (method == DELETE && this->checkMethod("DELETE"))
			deleteMethod(request);
		else
			throw ResponseException("405");

		log(logDEBUG) << "Response object succesfully created";
	}
	catch (std::exception &e)
	{
		// Handle other methods or send a 405 Method Not Allowed response
		this->_status = atoi(e.what());
		// this->_reason = getStatusMessage(e.what()); // function to get matchin reason for err code
		this->_reason = getStatusMessage(e.what()); // function to get matchin reason for err code
		this->_type = "text/html";
		this->_body = readFileToString(findError(e.what()));
		this->_connection = "close";
		this->_len = _body.length();
	}
}

/* Sets date and time to moment of copy */
Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_connection(copy._connection),
	_body(copy._body)
{}

// Destructor
Response::~Response()
{
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
	this->_loc = assign._loc;
	this->_servSet = assign._servSet;
	return (*this);
}

std::string Response::makeResponse()
{
	std::ostringstream response;

	response << HTTPVERSION << " " << this->_status << " " << this->_reason << "\r\n";
	response << "Date: " << getDateTime() << "\r\n";
	response << "Content-Length: " << this->_len << "\r\n";
	if (this->_type != "")
		response << "Content-Type: " << this->_type << "\r\n";
	response << "Connection: " << this->_connection << "\r\n";
	if (!this->_redir.empty())
		response << "Location: " << this->_redir << "\r\n";
	response << "\r\n";
	std::string return_value = response.str();
	if (this->_len)
		return_value += this->_body + "\r\n\r\n";
	return (return_value);
}

void	Response::postMethod(Request &request)
{
	int status = 0;
	createFiles(request, status);
	switch (status) {
		case 201:
			this->_status = 201;
			this->_reason = getStatusMessage("201");
			this->_type = "text/html";
			this->_connection = request.getConnection();
			this->_body = readFileToString("content/html/upload_success.html");
			this->_len = _body.length();
			break;
		case 500:
			throw ResponseException("500");
		case 400:
			throw ResponseException("400");
	}
}

void Response::createFiles(Request &request, int &status)
{
	std::string file = _servSet->root + "/" + _loc->path + "/";
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
	std::string filePath = this->extractFilePath(request);

	this->_status = 200;
	if (!filePath.empty())
	{
		this->_body = readFileToString(filePath);
		this->_type = findType(filePath);
	}
	else
		this->_type = "text/html";
	this->_len = _body.length();
	this->_reason = getStatusMessage("200");

	this->_connection = request.getConnection();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		throw ResponseException("404");
	}
}


void	Response::deleteMethod(Request &request)
{
	std::string file = _servSet->root + request.getLoc();

	if (remove(file.c_str()) != 0) {
		log(logERROR) << "Failed to delete file: " << file;
		throw ResponseException("404");
	}
	else {
		this->_status = 200;
		this->_reason = getStatusMessage("200");
		this->_type = "text/html";
		this->_connection = request.getConnection();
		this->_body = readFileToString("content/html/delete_success.html");
		this->_len = _body.length();
	}
}

/* Loops over all possible server locations and checks if they match the request location.
If no match was found, the first location in the map is used as default. */
Location Response::findLoc(const std::string& uri, ServerSettings &sett)
{
	std::map<std::string, Location>::const_iterator it = (sett.locations).begin();
	for (; it != sett.locations.end(); ++it)
	{
		if (it->first == uri)
		{
			Location loc = it->second;
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
		return sett.locations.at("/"); // Handle the case when no match is found - we need a smarter way of just returning item 0?
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
void	Response::createDirlisting(std::string dirPath)
{
	std::string htmlTemplate = DIRLIST;

	std::size_t pos = htmlTemplate.find("INSERT");
	std::string insertList = this->loopDir(dirPath);
	htmlTemplate.replace(pos, 6, insertList);

	while((pos = htmlTemplate.find("PATH")) != std::string::npos)
		htmlTemplate.replace(pos, 4, this->_loc->path);

	this->setBody(htmlTemplate);

}

// Function that loops through directory and subdirectories and
// creates html list of the content
std::string	Response::loopDir(std::string dirPath) {
	std::ostringstream html;
	if (dirPath[0] == '/' || dirPath[0] == '.') // Check if path starts with / or .
		dirPath = dirPath.substr(1);
	dirPath = this->_servSet->root + "/" + dirPath;

	dirPath = this->_servSet->root + "/" + dirPath;
	struct dirent	*entry;
	DIR		*dir = opendir(dirPath.c_str());
	if (dir == NULL) {
		log(logERROR) << "Error opening directory: " << dirPath;
		throw ResponseException("500");
	}

	// Loop through directory and create a list in html
	// html << "\n\t\t<ul>";
	html << "<ul>";
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			// html << "\n\t\t\t<li> "
			html << "<li> "
			<< entry->d_name
			<< "<button onclick=\"deleteFile('"
			<< entry->d_name
			<< "')\">Delete</button>"
			<< "</li>";
			if (entry->d_type == DT_DIR) {
				std::string	subPath = dirPath + "/" + entry->d_name;
				html << this->loopDir(subPath);
			}
		}
	}
	// html << "\n\t\t</ul>";
	html << "</ul>";

	if (closedir(dir) != 0) {
		log(logERROR) << "Error closing directory: " << dirPath;
		throw ResponseException("500");
	}
	return html.str();
}

// Returns path to correct error page
std::string Response::findError(std::string errorCode) {
	if (_loc->loc_error_pages.find(errorCode) != _loc->loc_error_pages.end())
		return _loc->root + "/" + _loc->loc_error_pages[errorCode];
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

// Function that returns the correct file path, based on the URI
// and the root and index from the config file. Returnd empty string
// in the case of directory listing
std::string Response::extractFilePath(Request &request) {
	// Find end of the location path in the URI
	std::size_t	i = request.getLoc().find(this->_loc->path) + this->_loc->path.length();
	std::string	file;

	if (i < request.getLoc().length())
		// If URI contains a filename extract it
		file = request.getLoc().substr(i);
	else
		// Else use index file
		file = _loc->index;

	std::string filePath;
	if (this->_loc->path == "/")
	{ 	// add root to path if needed
		if (file.find(this->_loc->root) != std::string::npos)
			filePath = file;
		else
			filePath = this->_loc->root + "/" + file;
	}
	else if (_loc->autoindex)
	{ 	// directory listing
		filePath = "";
		this->createDirlisting(_loc->path);
	}
	else
		// use path from URI
		filePath = this->_loc->path + "/" + file;

	return filePath;
}

// check if redir contains URL scheme, if so redirect to
// this URL
bool		Response::checkExternal() 
{
	if (this->_loc->redir.find("http") == 0 || this->_loc->redir.find("https") == 0)
	{
		this->_status = 302;
		this->_reason = "Found";
		this->_connection = "close";
		this->_redir = this->_loc->redir;
		this->_len = 0;

		return true;
	}

	return false;
}


void	Response::setStatus(int status) {
	this->_status = status;
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

std::string	Response::getConnection() {
	return this->_connection;
}



bool Response::isValidRequest(Request &request) {
	if (request.getLoc().find("..") != std::string::npos) {
		log(logERROR) << "Invalid request: path contains double dots.";
		return false;
	}
	if (request.getConnection() != "keep-alive" && request.getConnection() != "close") {
		log(logERROR) << "Invalid request: connection header is invalid:";
		return false;
	}
	else
		return true;
}
