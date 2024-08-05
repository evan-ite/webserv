#include "../includes/settings.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename)
{
	this->parseConfig(filename);
}

Config::Config(const Config &src)
{
	*this = src;
}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs)
{
	if (this != &rhs)
	{
		this->_Servers = rhs._Servers;
		this->_fallBackServer = rhs._fallBackServer;
	}
	return (*this);
}

/**
 * @brief Parses the server configuration from a file.
 *
 * This function attempts to load the server configuration from the specified file.
 * If an error occurs during the process, it logs the error message.
 * Initially, it loads a fallback configuration before reading the actual server configuration.
 *
 * @param filename The name of the configuration file to be parsed.
 */
void Config::parseConfig(const std::string &filename)
{
	try
	{
		loadFallback(DEFAULT_CONF);
		readServer(filename);
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}

/**
 * @brief Reads the server configuration from a file.
 *
 * It looks for server configuration blocks and processes them.
 *
 * @param filename The name of the file to read the server configuration from.
 * @throws std::runtime_error If the file cannot be opened.
 */
void Config::readServer(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	bool parseServer = false;
	std::string line;
	std::string server;
	int braceCount = 0;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file: " + filename);

	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
			parseServer = true;
		if (parseServer)
		{
			server.append(line + "\n");
			countBraces(line, &braceCount);

			if (braceCount == 0 && parseServer)
			{
				parseMultipleServers(server);
				server.clear();
				parseServer = false;
			}
		}
	}
}

/**
 * @brief Parses multiple server configurations from a single server block.
 *
 * This function extracts the ports and hosts from the server block and initializes
 * the temporary server configuration with fallback values. It then loads the user
 * configuration and creates new server settings for each combination of host and port.
 *
 * @param server The server block configuration as a string.
 */
void Config::parseMultipleServers(std::string server)
{
	std::vector<std::string> ports = getPorts(server);
	std::vector<std::string> hosts = getHosts(server);

	//initialize _tempServer with fallback values
	this->_tempServer = this->_fallBackServer;
	loadServerStruct(server); // load user configuration
	for (size_t i = 0; i < hosts.size(); ++i)
	{
		for (size_t j = 0; j < ports.size(); ++j)
		{
			ServerSettings newServer = this->_tempServer;

			newServer.host = hosts[i];
			std::istringstream iss(ports[j]);
			iss >> newServer.port;

			std::string key = hosts[i] + ":" + ports[j];
			this->_Servers.insert(std::make_pair(key, newServer));

			log(logINFO) << "Config for server " << key << " loaded";
		}
	}
}

/**
 * @brief Loads the server configuration from a configuration string.
 *
 * It parses the configuration string line by line, extracting key-value pairs
 * and handling location blocks. It updates the temporary server configuration with the
 * parsed values.
 *
 * @param configString The server configuration as a string.
 */
void Config::loadServerStruct(const std::string &configString)
{
	std::istringstream configStream(configString);
	std::string line;
	bool parsingLocation = false;
	Location currentLocation("/");

	while (std::getline(configStream, line))
	{
		std::istringstream iss(line);
		std::string key;
		std::string value;

		iss >> key >> value; // Assuming 'location /path/' format
		removeCharacter(value, ';');
		removeCharacter(value, '"');
		if (locationMode(line, &parsingLocation, &currentLocation, value))
			continue;
		if (!parsingLocation)
			parseServer(key, value, line);
		else
			parseLocation(&currentLocation, key, value, line);
	}
	if (parsingLocation)
		this->_tempServer.locations[currentLocation.path] = currentLocation;
}

/**
 * @brief Loads the fallback server configuration from a file.
 *
 * This function reads the fallback configuration file and loads its contents into the
 * temporary server configuration. It then sets the fallback server configuration and
 * generates status messages.
 *
 * @param filename The name of the fallback configuration file.
 * @throws std::runtime_error If the file cannot be opened.
 */
void Config::loadFallback(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::string line;
	std::string server;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open fallback file");

	while (std::getline(file, line))
		server.append(line + "\n");
	loadServerStruct(server);
	this->_fallBackServer = this->_tempServer;
	makeStatusMessages(_fallBackServer);
}

/* ============================== Getters =============================== */
ServerSettings Config::getServer(std::string serverIP) const //Throws an exception (std::out_of_range) if the key doesn't exist in the map.
{
	return (this->_Servers.at(serverIP));
}

std::map<std::string, ServerSettings> Config::getServersMap(void) const
{
	return (this->_Servers);
}

//################################################ Testing functions ################################################
void Config::printServers(void) const
{
	for (std::map<std::string, ServerSettings>::const_iterator serverPair = _Servers.begin(); \
		serverPair != _Servers.end(); ++serverPair)
	{
		std::cout << serverPair->second << std::endl;
	}
}

void Config::printFallback(void) const
{
	std::cout << this->_fallBackServer << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Location& location)
{
	os << " Path: " << location.path << std::endl;
	os << " Root: " << location.root << std::endl;
	os << " Index: " << location.index << std::endl;
	os << " Redirection: " << location.redir << std::endl;
	os << " Autoindex: " << (location.autoindex ? "true" : "false") << std::endl;
	os << " Allow:" << std::endl;
	for (std::vector<std::string>::const_iterator it = location.allow.begin(); it != location.allow.end(); ++it)
		os << "  - " << *it << std::endl;
	os << " Error Pages: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = location.loc_error_pages.begin(); it != location.loc_error_pages.end(); ++it)
		os << " " << it->first << ": " << it->second << std::endl;
	return (os);
}

std::ostream& operator<<(std::ostream& os, const ServerSettings& server)
{
	os << "================ Server ===================" << std::endl;
	os << "Root: " << server.root << std::endl;
	os << "Host: " << server.host << std::endl;
	os << "Port: " << server.port << std::endl;
	os << "Client Max Body Size: " << server.client_max_body_size << std::endl;
	os << "Client Body In File Only: " << (server.client_body_in_file_only ? "true" : "false") << std::endl;
	os << "Client Body Buffer Size: " << server.client_body_buffer_size << std::endl;
	os << "Client Body Timeout: " << server.client_body_timeout << std::endl;
	os << "CGI: " << (server.cgi ? "true" : "false") << std::endl;
	os << "CGI Extension: " << server.cgi_extension << std::endl;
	os << "CGI Bin: " << server.cgi_bin << std::endl;
	os << "Dir List Template: " << server.dirlistTemplate.size() << std::endl;

	os << "Error Pages:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it)
	{
		os << " " << it->first << ": " << it->second << std::endl;
	}

	os << "Locations:" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it)
	{
		os << "============ Location " << it->first << " ==============="<< std::endl << it->second;
	}
	return (os);
}
