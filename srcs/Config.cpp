#include "../includes/Config.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename)
{
	try
	{
		if (filename == DEFAULT_CONF)
			readServer(filename, false);
		else {
			readServer(DEFAULT_CONF, true);
			readServer(filename, false);
		}
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}

Config::Config(const Config &src) {*this = src;}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs)
{
	if (this != &rhs) {
		_Servers = rhs._Servers;
		_fallBack = rhs._fallBack;
	}
	return (*this);
}

/**
 * @brief Reads and parses server configuration from a file.
 *
 * This function reads a server configuration from the specified file and parses it.
 * It handles comments, empty lines, and server blocks. If the server block is the default,
 * it loads the server structure and sets it as the fallback server. Otherwise, it parses
 * multiple servers.
 *
 * @param filename The name of the configuration file to read.
 * @param isFallBack A boolean indicating whether the server block is the default server.
 *
 * @throws std::runtime_error If the file cannot be opened.
 */
void Config::readServer(const std::string &filename , bool isFallBack)
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
		line = trimLeadingWhitespace(line);
		if (startsWith(line, "#") || line.empty())
			continue;	// skip comments and empty lines
		if (startsWith(line, "server"))
			parseServer = true;
		if (parseServer)
		{
			server.append(line + "\n");
			countBraces(line, &braceCount);
			if (braceCount == 0 && isFallBack){
				loadServerStruct(server);
				_fallBack = _tempServer;
			}
			else if (braceCount == 0 && parseServer)
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
void Config::parseMultipleServers(std::string serverString)
{
	std::vector<std::string> ports = getPortHost(serverString, "listen");
	std::vector<std::string> hosts = getPortHost(serverString, "host");

	_tempServer = _fallBack;
	loadServerStruct(serverString); // load user configuration
	for (size_t i = 0; i < hosts.size(); ++i)
	{
		for (size_t j = 0; j < ports.size(); ++j)
		{
			Server newServer = _tempServer;

			newServer.setHost(hosts[i]);
			int port;
			std::istringstream iss(ports[j]);
			iss >> port;
			if (iss.fail())
				throw std::runtime_error("Error: invalid port value");
			newServer.setPort(port);

			std::string key = hosts[i] + ":" + toString(port);
			_Servers.insert(std::make_pair(key, newServer));

			log(logDEBUG) << "Config for server " << key << " loaded";
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
			parse(_tempServer, key, value, line);
		else
			parse(currentLocation, key, value, line);
		parseUnique(currentLocation, parsingLocation, key, value, line);
	}
	if (parsingLocation)
		_tempServer.addLocation(currentLocation);
}

std::map<std::string, Server> Config::getServersMap(void) const {return (_Servers);}

