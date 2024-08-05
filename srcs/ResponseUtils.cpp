#include "../includes/settings.hpp"

/**
 * @brief Checks if the request is valid.
 *
 * This function validates the request by checking if the location contains double dots
 * and if the connection header is either "keep-alive" or "close".
 *
 * @param request The HTTP request object.
 * @return True if the request is valid, false otherwise.
 */
bool Response::isValidRequest(Request &request)
{
	if (request.getLoc().find("..") != std::string::npos)
	{
		log(logERROR) << "Invalid request: path contains double dots.";
		return (false);
	}
	if (request.getConnection() != "keep-alive" && request.getConnection() != "close")
	{
		if (request.getConnection().empty())
		{
			request.setConnection("keep-alive");
			return (true);
		}
		else
		{
			log(logERROR) << "Invalid request: connection header is invalid:";
			return (false);
		}
	}
	else
		return (true);
}


/**
 * @brief Checks if the redirection is to an external URL.
 *
 * This function checks if the redirection URL starts with "http" or "https".
 * If so, it sets the response status to 302 (Found) and prepares the redirection.
 *
 * @return True if the redirection is to an external URL, false otherwise.
 */
bool Response::checkExternal()
{
	if (this->_loc->redir.find("http") == 0 || this->_loc->redir.find("https") == 0)
	{
		this->_status = 302;
		this->_reason = "Found";
		this->_connection = "close";
		this->_redir = this->_loc->redir;
		this->_len = 0;

		return (true);
	}

	return (false);
}

/**
 * @brief Extracts the file path from the request.
 *
 * This function determines the correct file path based on the URI, root, and index
 * from the configuration file. It returns an empty string in the case of directory listing.
 *
 * @param request The HTTP request object.
 * @return The extracted file path as a string.
 */
std::string Response::extractFilePath(Request &request)
{
	// Find end of the location path in the URI
	std::size_t	i = request.getLoc().find(this->_loc->path) + this->_loc->path.length();
	std::string	file;

	if (i < request.getLoc().length())
	{ // If URI contains a filename extract it
		file = request.getLoc().substr(i);
		if (file.find('.') == std::string::npos)
			file += "/" + _loc->index;
	}
	else // Else use index file
		file = _loc->index;

	if (file[0] && file[0] == '/')
		file = file.substr(1);

	std::string filePath;
	if (_loc->autoindex)
	{ // directory listing
		filePath = "";
		this->createDirlisting(_loc->path);
	}
	else
	{
		if (file.find(this->_loc->root) != std::string::npos)
			filePath = file;
		else
			filePath = this->_loc->root + "/" + file;
	}

	return (filePath);
}


/**
 * @brief Finds the error page path for a given error code.
 *
 * This function returns the path to the correct error page based on the error code.
 * It first checks the location-specific error pages, then the server-wide error pages.
 *
 * @param errorCode The error code as a string.
 * @return The path to the error page as a string.
 */
std::string Response::findError(std::string errorCode)
{
	if (_loc->loc_error_pages.find(errorCode) != _loc->loc_error_pages.end())
		return (_loc->root + "/" + _loc->loc_error_pages[errorCode]);
	else if (_servSet->error_pages.find(errorCode) != _servSet->error_pages.end())
		return (_servSet->error_pages[errorCode]);
	else
		return (_servSet->error_pages["500"]);
}

/**
 * @brief Gets the status message for a given status code.
 *
 * This function returns the appropriate status message for the provided status code.
 * If no specific message is found, it defaults to "Internal Server Error".
 *
 * @param statusCode The status code as a string.
 * @return The status message as a string.
 */
std::string Response::getStatusMessage(std::string statusCode)
{
	if (_servSet->error_messages.find(statusCode) != _servSet->error_messages.end())
		return (_servSet->error_messages[statusCode]);
	else
		return ("Internal Server Error");
}


/**
 * @brief Finds the matching location for a given URI.
 *
 * This function iterates over all possible server locations and checks if they match
 * the request location. If no match is found, the first location in the map is used as default.
 *
 * @param uri The request URI.
 * @param sett The server settings object.
 * @return The matching location object.
 */
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
		return (sett.locations.at("/")); // Handle the case when no match is found - we need a smarter way of just returning item 0?
}

/**
 * @brief Checks if the HTTP method is allowed in the location.
 *
 * This function checks if the provided HTTP method is allowed in the current location.
 *
 * @param method The HTTP method in capital letters.
 * @return True if the method is allowed, false otherwise.
 */
bool	Response::checkMethod(std::string method)
{
	Location loc = *this->_loc;
	std::vector<std::string>::iterator it = std::find(loc.allow.begin(), loc.allow.end(), method);

	if (it != loc.allow.end())
		return (true);
	return (false);
}

/**
 * @brief Creates files based on the request data.
 *
 * This function creates files in the server's root directory based on the data provided
 * in the request. It sets the response status accordingly.
 *
 * @param request The HTTP request object.
 * @param status The status code to be set based on the success or failure of file creation.
 */
void Response::createFiles(Request &request, int &status)
{
	std::string file = _servSet->root + "/" + _loc->path + "/";
	std::vector<std::pair<std::string, std::string> > fileData = request.getFileData();

	if (fileData.empty())
	{
		log(logERROR) << "Bad request: no files to create";
		status = 400;
		return ;
	}
	bool anyFailure = false;
	for (size_t i = 0; i < fileData.size(); ++i)
	{
		std::string filename = fileData[i].first;
		std::string content = fileData[i].second;
		std::string fullpath = file + filename;
		std::ofstream file(fullpath.c_str(), std::ios::binary);

		if (!file)
		{
			log(logERROR) << "Failed to open file for writing: " << fullpath;
			anyFailure = true;
			continue;
		}
		file.write(content.data(), content.size());
		if (!file.good())
		{
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

/**
 * @brief Checks if the MIME type for the given extension is valid.
 *
 * This function iterates over the possible extensions in MIME.txt and checks if the
 * provided extension is valid. If found, it returns the corresponding content type.
 *
 * @param extension The file extension.
 * @return The content type as "type/subtype". Defaults to "text/plain" if no match is found.
 */
std::string checkMime(const std::string &extension)
{
	std::ifstream mime(MIMEFILE);
	if (!mime.is_open())
	{
		log(logERROR) << "Error opening file: " << MIMEFILE;
		return ("");
	}

	std::string line;
	while (std::getline(mime, line))
	{
		std::string::size_type sep = line.find(',');
		std::string temp = line.substr(0, sep);
		if (temp == extension)
		{
			mime.close();
			return (line.substr(sep + 1));
		}
	}
	mime.close();
	return ("text/plain");
}

/**
 * @brief Finds the content type for a given filename.
 *
 * This function checks the file extension of the provided filename and returns the
 * corresponding content type. If no match is found, it defaults to "text/plain".
 *
 * @param filename The name of the file.
 * @return The content type as "type/subtype".
 */
std::string findType(const std::string &filename)
{
	std::size_t i = filename.rfind('.');
	if (i == std::string::npos)
		return "text/plain";
	std::string extension = filename.substr(i + 1);

	return checkMime(extension);
}
