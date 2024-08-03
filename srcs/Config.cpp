#include "../includes/settings.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename)
{
	try
	{
		this->loadFallback(DEFAULT_CONF);
		this->readServer(filename);
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}

Config::Config(const Config &src)
{
	*this = src;
}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs)
{
	if (this != &rhs)
		this->_Servers = rhs._Servers;
	return (*this);
}

/* reads one entire server section from config file */
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
		if (line.find("server") != std::string::npos) // change this to starts_with!
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
	if (filename == DEFAULT_CONF && !this->_Servers.empty())
		this->_Servers["default"] = this->_Servers.begin()->second;
}

void Config::parseMultipleServers(std::string serverString)
{
	std::vector<std::string> ports = this->getPortHost(serverString, "listen");
	std::vector<std::string> hosts = this->getPortHost(serverString, "host");

	//initialize _tempServer with fallback values
	this->_tempServer = this->_Servers["default"];

	// load user configuration
	loadServerStruct(serverString);

	for (size_t i = 0; i < hosts.size(); ++i)
	{
		for (size_t j = 0; j < ports.size(); ++j)
		{
			Server newServer = this->_tempServer;

			newServer.setHost(hosts[i]);
			int port;
			std::istringstream iss(ports[j]);
			iss >> port;
			newServer.setPort(port);

			std::string key = hosts[i] + ":" + ports[j];
			this->_Servers.insert(std::make_pair(key, newServer));

			log(logINFO) << "Config for server " << key << " loaded";
		}
	}
}

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
			parse(&(this->_tempServer), key, value, line);
		else
			parse(&currentLocation, key, value, line);
	}
	if (parsingLocation)
		this->_tempServer.addLocation(currentLocation);
}

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
	this->_Servers.insert(std::make_pair("default", this->_tempServer));
}

Server Config::getServer(std::string serverHost) const // Throws an exception (std::out_of_range) if the key doesn't exist in the map.
{
	return (this->_Servers.at(serverHost));
}

std::map<std::string, Server> Config::getServersMap(void) const
{
	std::map<std::string, Server> servers = this->_Servers;
	if (servers.find("default") != servers.end())
		servers.erase("default");
	return servers;
}

void Config::printServers(void) const
{
	for (std::map<std::string, Server>::const_iterator serverPair = _Servers.begin(); \
		serverPair != _Servers.end(); ++serverPair)
	{
		std::cout << serverPair->second << std::endl;
	}
}
