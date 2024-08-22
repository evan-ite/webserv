#include "../includes/Config.hpp"

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
		_tempServer.addLocation(*currentLocation);
		*parsingLocation = false;
		return (true);
	}
	if (line.find("location") != std::string::npos)
	{
		if (_tempServer.locationExists(value))
			*currentLocation = _tempServer.findLocation(value);
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
 * @brief Parses a configuration setting and updates the provided ASetting object (Derived Classes).
 *
 * This function takes a key-value pair from a configuration file and updates the
 * corresponding attribute in the provided ASetting object. It handles various
 * configuration keys such as "root", "index", "error_page", "autoindex", "allow",
 * "client_max_body_size", "cgi", "cgi_extension", "cgi_bin", "dir_list", and "cgi_pass".
 *
 * @param setting The ASetting object to be updated.
 * @param key The configuration key.
 * @param value The configuration value.
 * @param line The entire line from the configuration file, used for parsing complex values.
 *
 * @throws std::runtime_error If the key is "error_page" and the error code or error page is invalid.
 * @throws std::runtime_error If the key is "client_max_body_size" and the size is invalid.
 */
void Config::parse(ASetting& setting, std::string key, std::string value, std::string line)
{
	if (key == "root")
		setting.setRoot(value);
	else if (key == "index")
		setting.setIndex(value);
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
		setting.addErrorPage(error_code, error_page);
	}
	else if (key == "autoindex")
		setting.setAutoindex((value == "on"));
	else if (key == "allow")
	{
		std::istringstream iss(line);
		std::string allow;
		iss >> allow;
		while (iss >> allow)
		{
			removeCharacter(allow, ';');
			setting.addAllow(allow);
		}
	}
	else if (key == "client_max_body_size")
	{
		std::istringstream iss(value);
		int size;
		if (!(iss >> size))
			throw std::runtime_error("Error: invalid client_max_body_size");
		setting.setClientMaxBodySize(size);
	}
	else if (key == "cgi")
		setting.setCgi((value == "on"));
	else if (key == "cgi_extension")
		setting.setCgiExtension(value);
	else if (key == "cgi_bin")
		setting.setCgiBin(value);
	else if (key == "dir_list")
		setting.setDirlistTemplate(value);
	else if (key == "cgi_pass")
		setting.setCgiPass(value);
}

/**
 * @brief Parses the unique configurations of the derived classess of ASetting (Server/Location).
 *
 * This function takes a key-value pair from a configuration file and updates the
 * corresponding attribute in the provided Location or Server object.
 *
 * @param location The Location object to be updated if isLoc is true.
 * @param isLoc A boolean indicating whether the key-value pair pertains to a Location object.
 * @param key The configuration key.
 * @param value The configuration value.
 * @param line The entire line from the configuration file, used for parsing complex values.
 *
 * @throws std::runtime_error If the key is "return" and the redirection value is invalid.
 * @throws std::runtime_error If the key is "listen" and the port number is invalid.
 */
void Config::parseUnique(Location &location, bool isLoc, std::string key, std::string value, std::string line)
{
	if (isLoc)
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
			location.setRedir(redir);
		}
	}
	else
	{
		if (key == "host")
			_tempServer.setHost(value);
		else if (key == "listen")
		{
			std::istringstream iss(value);
			int port;
			if (!(iss >> port) || port < 0)
				throw std::runtime_error("Error: invalid port number or port in location block");
			_tempServer.setPort(port);
		}
		else if (key == "server_name")
			_tempServer.setServerName(value);
	}
}
