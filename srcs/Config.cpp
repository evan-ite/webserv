#include "../includes/server.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename) {
	this->parseConfig(filename);
}

Config::Config(const Config &src) {
	*this = src;
}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs) {
	if (this != &rhs) {
		this->_Servers = rhs._Servers;
		this->_fallBackServer = rhs._fallBackServer;
	}
	return *this;
}

void Config::parseLocation(Location *currentLocation, std::string key, std::string value) {
	if (key == "root")
		currentLocation->root = value;
	else if (key == "index")
		currentLocation->index = value;
	else if (key == "error_page")
		currentLocation->error_page = value;
	else if (key == "cgi")
		currentLocation->cgi = value;
	else if (key == "client_max_body_size")
		currentLocation->client_max_body_size = value;
	else if (key == "allow_uploads")
		currentLocation->allow_uploads = (value == "on");
}

void Config::parseServer(std::string key, std::string value) {
	if (key == "server_name")
		this->_tempServer.server_name = value;
	else if (key == "root")
		this->_tempServer.root = value;
	else if (key == "host")
		this->_tempServer.host = value;
	else if (key == "listen") {
		std::istringstream iss(value);
		int port;
		if (!(iss >> port))
			throw std::runtime_error("Error: invalid port number");
		this->_tempServer.port = port;
	}
}

bool Config::locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value) {
	if (line.find("}") != std::string::npos && *parsingLocation)
	{
		this->_tempServer.locations[currentLocation->path] = *currentLocation;
		log(logDEBUG) << "saved path config for " << currentLocation->path;
		*parsingLocation = false;
		return (true);
	}
	if (line.find("location") != std::string::npos)
	{
		*currentLocation = Location(value);
		*parsingLocation = true;
		return (true);
	}
	return (false);
}

void Config::countBraces(std::string line, int *braceCount)
{
	size_t openBraces = std::count(line.begin(), line.end(), '{');
	size_t closeBraces = std::count(line.begin(), line.end(), '}');
	*braceCount += openBraces - closeBraces;
}

void Config::loadServerStruct(const std::string &configString) {
	std::istringstream configStream(configString);
	std::string line;
	bool parsingLocation = false;
	Location currentLocation("/");

	while (std::getline(configStream, line)) {
		std::istringstream iss(line);
		std::string key;
		std::string value;

		iss >> key >> value; // Assuming 'location /path/' format
		if (!value.empty() && value[value.size() - 1] == ';')
			value.erase(value.size() - 1);
		if (locationMode(line, &parsingLocation, &currentLocation, value))
			continue;
		if (!parsingLocation)
			parseServer(key, value);
		else
			parseLocation(&currentLocation, key, value);
	}
	if (parsingLocation)
		this->_tempServer.locations[currentLocation.path] = currentLocation;
}

std::vector<std::string> Config::getPorts(std::string server) {
	std::vector<std::string> ports;
	size_t startPos = 0;

	while (true) {
		size_t pos = server.find("listen", startPos);
		if (pos == std::string::npos) break;
		size_t endPos = server.find(";", pos);
		if (endPos == std::string::npos) break;

		std::string portStr = server.substr(pos + 7, endPos - pos - 7);
		std::istringstream iss(portStr);
		std::string port;
		while (iss >> port)
			ports.push_back(port);
		startPos = endPos + 1;
	}
	// ONLY FOR TESTING
/* 	for (size_t i = 0; i < ports.size(); ++i) {
		std::cout << "Port " << i + 1 << ": " << ports[i] << std::endl;
	} */
	return ports;
}

std::vector<std::string> Config::getHosts(std::string server) {
	std::vector<std::string> hosts;
	size_t startPos = 0;

	while (true) {
		size_t pos = server.find("host", startPos);
		if (pos == std::string::npos || !isblank(server[pos + 4])) break;
		size_t endPos = server.find(";", pos);
		if (endPos == std::string::npos) break;

		std::string hostStr = server.substr(pos + 5, endPos - pos - 5);
		std::istringstream iss(hostStr);
		std::string host;
		while (iss >> host)
			hosts.push_back(host);
		startPos = endPos + 1;
	}
		// ONLY FOR TESTING
/* 	for (size_t i = 0; i < hosts.size(); ++i) {
		std::cout << "Host " << i + 1 << ": " << hosts[i] << std::endl;
	} */
	return hosts;
}

void Config::parseMultipleServers(std::string server)
{
	std::vector<std::string> ports = getPorts(server);
	std::vector<std::string> hosts = getHosts(server);

	//initialize _tempServer with fallback values
	this->_tempServer = this->_fallBackServer;
	loadServerStruct(server); // load user configuration
	for (size_t i = 0; i < hosts.size(); ++i) {
		for (size_t j = 0; j < ports.size(); ++j) {
			Server newServer = this->_tempServer;

			newServer.host = hosts[i];
			std::istringstream iss(ports[j]);
			iss >> newServer.port;

			std::string key = hosts[i] + ":" + ports[j];
			this->_Servers.insert(std::make_pair(key, newServer));

			log(logDEBUG) << "Server " << key << " loaded";
		}
	}
}

void Config::readServer(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	bool parseServer = false;
	std::string line;
	std::string server;
	int braceCount = 0;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file");

	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
			parseServer = true;
		if (parseServer) {
			server.append(line + "\n");
			countBraces(line, &braceCount);

			if (braceCount == 0 && parseServer) {
				parseMultipleServers(server);
				server.clear();
				parseServer = false;
			}
		}
	}
}

void Config::loadFallback(const std::string &filename) {
	std::ifstream file(filename.c_str());
	std::string line;
	std::string server;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open fallback file");

	while (std::getline(file, line))
		server.append(line + "\n");
	loadServerStruct(server);
	this->_fallBackServer = this->_tempServer;
}

void Config::parseConfig(const std::string &filename) {
	try {
		loadFallback(FALLBACK_CONF);
		readServer(filename);
	}
	catch (std::exception &e) {
		log(logERROR) << e.what();
	}
}

/* Server Config::getServer() const { // TO BE DELETED
	return this->_tempServer;
} */

Server Config::getServer(std::string serverIP) const { //Throws an exception (std::out_of_range) if the key doesn't exist in the map.
	return this->_Servers.at(serverIP);
}

std::map<std::string, Server> Config::getServersMap(void) const {
	return this->_Servers;
}

/* std::ostream& operator<<(std::ostream& os, const Config& obj) { // For testing purposes ONLY.
	for (std::map<std::string, Server>::const_iterator serverPair = obj._Servers.begin(); \
		serverPair != obj._Servers.end(); ++serverPair) {
		const Server& server = serverPair->second;
		os << "Server Name: " << server.server_name << std::endl;
		os << "Root: " << server.root << std::endl;
		os << "Host: " << server.host << std::endl;
		os << "Port: " << server.port << std::endl;
		os << "Locations:" << std::endl;
		for (std::map<std::string, Location>::const_iterator locationPair = server.locations.begin(); \
			locationPair != server.locations.end(); ++locationPair) {
			const Location& location = locationPair->second;
			os << "  Path: " << location.path << std::endl;
			os << "    Root: " << location.root << std::endl;
			os << "    Index: " << location.index << std::endl;
			os << "    Error Page: " << location.error_page << std::endl;
			os << "    CGI: " << location.cgi << std::endl;
			os << "    Client Max Body Size: " << location.client_max_body_size << std::endl;
			os << "    Allow Uploads: " << (location.allow_uploads ? "true" : "false") << std::endl;
		}
	}
	return os;
} */


/* TO-DO
X extract a single server block.
X read the ports. (external function)
X Go through the server string and store configuration in the server struct. (modify exiting function).
X loop through the ports:
	X create a copy of the Server struct.
	X change Port and Host to match the current port.
	X insert the new pair in the map.
X load the default configuration before loading the user configuration.
- apply convertions where it is necessary.
*/
