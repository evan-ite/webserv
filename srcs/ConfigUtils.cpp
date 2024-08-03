#include "../includes/settings.hpp"

static void serverBlock(Server* current, std::string key, std::string value);
static void locationBlock(Location* current, std::string value, std::string line);

/**
 * Counts the number of open and close braces in a given line and updates the brace count.
 *
 * @param line The line to count the braces in.
 * @param braceCount A pointer to the brace count variable to update.
 */
void Config::countBraces(std::string line, int *braceCount)
{
	size_t openBraces = std::count(line.begin(), line.end(), '{');
	size_t closeBraces = std::count(line.begin(), line.end(), '}');
	*braceCount += openBraces - closeBraces;
}

/**
 * Determines the location mode based on the given line and updates the current location accordingly.
 *
 * @param line The line to analyze.
 * @param parsingLocation A pointer to a boolean indicating whether the function is currently parsing a location block.
 * @param currentLocation A pointer to the current location object.
 * @param value The value associated with the location.
 * @return True if the location mode was successfully determined and updated, false otherwise.
 */
bool Config::locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value)
{
	if (line.find("}") != std::string::npos && *parsingLocation)
	{
		this->_tempServer.addLocation(*currentLocation);
		*parsingLocation = false;
		return (true);
	}
	if (line.find("location") != std::string::npos)
	{
		if (this->_tempServer.locationExists(value))
			*currentLocation = this->_tempServer.findLocation(value);
		else
			*currentLocation = Location(value);
		*parsingLocation = true;
		return (true);
	}
	return (false);
}

/**
 * @brief Extracts port and host values from a server string based on a keyword.
 *
 * This function searches for the specified keyword in the server string and extracts the values
 * that follow the keyword until a semicolon is encountered. The extracted values are then stored
 * in a vector and returned.
 *
 * @param server The server string to search in.
 * @param keyword The keyword to search for in the server string.
 * @return A vector containing the extracted port and host values.
 */
std::vector<std::string> Config::getPortHost(const std::string& server, const std::string& keyword)
{
	std::vector<std::string> values;
	size_t startPos = 0;

	while (true)
	{
		size_t pos = server.find(keyword, startPos);
		if (pos == std::string::npos) break;
		size_t endPos = server.find(";", pos);
		if (endPos == std::string::npos) break;

		std::string valueStr = server.substr(pos + keyword.length(), endPos - pos - keyword.length());
		std::istringstream iss(valueStr);
		std::string value;
		while (iss >> value)
			values.push_back(value);
		startPos = endPos + 1;
	}

	return values;
}

/**
 * @brief Handles the "return" key in a location block.
 *
 * This function parses the provided line and extracts the redirection value.
 * The redirection value is then set in the current Location object.
 *
 * @param current Pointer to the current Location object.
 * @param key The key to be handled.
 * @param line The line containing the redirection value.
 *
 * @throws std::runtime_error if the redirection value is invalid.
 */
static void locationBlock(Location* current, std::string key, std::string line)
{
	if (key == "return")
	{
		std::istringstream iss(line);
		std::string redir;
		iss >> redir;
		iss >> redir;
		if (!(iss >> redir))
			throw std::runtime_error("Error: invalid redirection value");
		removeCharacter(redir, ';');
		removeCharacter(redir, '"');
		current->setRedir(redir);
	}
}

/**
 * @brief Updates the server configuration based on the given key-value pair.
 *
 * @param current Pointer to the Server object to be updated.
 * @param key The key representing the configuration parameter.
 * @param value The value representing the configuration value.
 * @throws std::runtime_error if an invalid port number or port in location block is encountered.
 */
static void serverBlock(Server* current, std::string key, std::string value)
{
	if (key == "host")
		current->setHost(value);
	else if (key == "listen")
	{
		std::istringstream iss(value);
		int port;
		if (!(iss >> port))
			throw std::runtime_error("Error: invalid port number or port in location block");
		current->setPort(port);
	}
}


/**
 * Parses the configuration settings for a given ASetting object.
 *
 * @param ptr The pointer to the ASetting object, either Server or Location.
 * @param key The key of the configuration setting.
 * @param value The value of the configuration setting.
 * @param line The entire line of the configuration setting.
 *
 * @throws std::runtime_error if the ptr is not of type Server or Location.
 * @throws std::runtime_error if an invalid error code or error page is encountered.
 * @throws std::runtime_error if an invalid client_max_body_size is encountered.
 */
void Config::parse(ASetting* ptr, std::string key, std::string value, std::string line)
{
	Server* serverPtr = dynamic_cast<Server *>(ptr);
	Location* locationPtr = dynamic_cast<Location *>(ptr);

	if (serverPtr)
		serverBlock(serverPtr, key, value);
	else if (locationPtr)
		locationBlock(locationPtr, key, line);
	else
		throw std::runtime_error("Error: Ptr mus be Server or Location");

	if (key == "root")
		ptr->setRoot(value);
	else if (key == "index")
		ptr->setIndex(value);
	else if (key == "error_page")
	{
		std::istringstream iss(line);
		std::string error_code;
		std::string error_page;
		iss >> error_code;
		if (!(iss >> error_code))
			throw std::runtime_error("Error: invalid error code");
		if (!(iss >> error_page))
			throw std::runtime_error("Error: invalid error page");
		removeCharacter(error_page, ';');
		removeCharacter(error_page, '"');
		ptr->addErrorPage(error_code, error_page);
	}
	else if (key == "allow_uploads")
		ptr->setAllowUploads((value == "on"));
	else if (key == "autoindex")
		ptr->setAutoindex((value == "on"));
	else if (key == "allow")
	{
		std::istringstream iss(line);
		std::string allow;
		iss >> allow;
		while (iss >> allow)
		{
			removeCharacter(allow, ';');
			ptr->addAllow(allow);
		}
	}
	else if (key == "client_max_body_size")
	{
		std::istringstream iss(value);
		int size;
		if (!(iss >> size))
			throw std::runtime_error("Error: invalid client_max_body_size");
		ptr->setClientMaxBodySize(size);
	}
	else if (key == "cgi")
		ptr->setCgi((value == "on"));
	else if (key == "cgi_extension")
		ptr->setCgiExtension(value);
	else if (key == "cgi_bin")
		ptr->setCgiBin(value);
	else if (key == "dir_list")
	{
		std::string path = ptr->getRoot() + "/" + value;
		ptr->setDirlistTemplate(path);
	}
	else if (key == "cgi_pass")
		ptr->setCgiPass(value);
}
