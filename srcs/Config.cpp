#include "../includes/server.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename)
{
	this->parseConfigFile(filename);
	this->readServer(filename);
}

Config::Config(const Config &src) {
	*this = src;
}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs) {
	if (this != &rhs)
		this->_Server = rhs._Server;
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
		this->_Server.server_name = value;
	else if (key == "host")
		this->_Server.host = value;
	else if (key == "listen") {
		std::istringstream iss(value);
		int port;
		if (!(iss >> port))
			throw std::runtime_error("Error: invalid port number");
		this->_Server.port = port;
	}
}

bool Config::locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value) {
	if (line.find("}") != std::string::npos && *parsingLocation)
	{
		this->_Server.locations[currentLocation->path] = *currentLocation;
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

void Config::parseConfigFile(const std::string &filename) {
	std::ifstream file(filename.c_str());
	std::string line;
	bool parsingLocation = false;
	Location currentLocation("/");

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file");

	while (std::getline(file, line))
	{
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
		this->_Server.locations[currentLocation.path] = currentLocation;
	file.close();
}

void Config::countBraces(std::string line, int *braceCount)
{
	size_t openBraces = std::count(line.begin(), line.end(), '{');
	size_t closeBraces = std::count(line.begin(), line.end(), '}');
	*braceCount += openBraces - closeBraces;
}

void Config::readServer(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	bool parseServer = false;
	std::string line;
	std::string server;
	int braceCount = 0;
	int i = 0;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file");

	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)
			parseServer = true;
		if (parseServer) {
			server.append(line + "\n");
			countBraces(line, &braceCount);

			if (braceCount == 0 && parseServer) { //
				i++;
				std::cout << "Server " << i << ":" << server << std::endl;
				server.clear();
				parseServer = false;
			}
		}
	}
}

/* TO-DO
- extract a single server block.
- read the ports. (external function)
- Go through the server string and store configuration in the server struct. (modify exiting function).
- loop through the ports:
	- create a copy of the Server struct.
	- change Port and Host to match the current port.
	- insert the new pair in the map.
*/

































Server Config::getServer(void) const {
	return this->_Server;
}

std::ostream& operator<<(std::ostream& os, const Config& obj) {
	const Server& Server = obj.getServer();
	os << "Server Name: " << Server.server_name << std::endl;
	os << "Host: " << Server.host << std::endl;
	os << "Port: " << Server.port << std::endl;
	os << "Locations:" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = Server.locations.begin(); it != Server.locations.end(); ++it) {
		const Location& location = it->second;
		os << "  Path: " << location.path << std::endl;
		os << "    Root: " << location.root << std::endl;
		os << "    Index: " << location.index << std::endl;
		os << "    Error Page: " << location.error_page << std::endl;
		os << "    CGI: " << location.cgi << std::endl;
		os << "    Client Max Body Size: " << location.client_max_body_size << std::endl;
		os << "    Allow Uploads: " << (location.allow_uploads ? "true" : "false") << std::endl;
	}
	return os;
}
