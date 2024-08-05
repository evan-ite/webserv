#include "../includes/settings.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename)
{
	try
	{
		this->readServer(DEFAULT_CONF);
		this->_Servers.clear();
		this->_Servers["default"] = this->_tempServer;
		this->readServer(filename);
		// log(logDEBUG) << "DEFAULT \n" << this->_Servers.at("default");
		// log(logDEBUG) << "size " << this->_Servers.size();
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
		line = trimLeadingWhitespace(line);
		if (startsWith(line, "#") || line.empty())
			continue;	// skip comments and empty lines
		if (startsWith(line, "server"))
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

void Config::parseMultipleServers(std::string serverString)
{
	std::vector<std::string> ports = this->getPortHost(serverString, "listen");
	std::vector<std::string> hosts = this->getPortHost(serverString, "host");

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

std::map<std::string, Server> Config::getServersMap(void) const
{
	std::map<std::string, Server> servers = this->_Servers;
	if (servers.find("default") != servers.end())
		servers.erase("default");
	return (servers);
}
