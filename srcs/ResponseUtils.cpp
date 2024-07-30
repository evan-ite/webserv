#include "../includes/settings.hpp"

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

// check if redir contains URL scheme, if so redirect to
// this URL
bool Response::checkExternal()
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


// Returns path to correct error page
std::string Response::findError(std::string errorCode) {
	if (_loc->loc_error_pages.find(errorCode) != _loc->loc_error_pages.end())
		return _loc->root + "/" + _loc->loc_error_pages[errorCode];
	else if (_servSet->error_pages.find(errorCode) != _servSet->error_pages.end())
		return _servSet->error_pages[errorCode];
	else
		return _servSet->error_pages["500"];
}

// Returns the correct status message for the given status code
std::string Response::getStatusMessage(std::string statusCode) {
	if (_servSet->error_messages.find(statusCode) != _servSet->error_messages.end())
		return _servSet->error_messages[statusCode];
	else
		return "Internal Server Error";
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
