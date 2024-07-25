#include "../includes/settings.hpp"

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

void Config::parseLocation(Location *currentLocation, std::string key, std::string value, std::string line) {
	if (key == "root")
		currentLocation->root = value;
	else if (key == "index")
		currentLocation->index = value;
	else if (key == "error_page") {
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
		currentLocation->loc_error_pages[error_code] = error_page;
	}
	else if (key == "allow_uploads")
		currentLocation->allow_uploads = (value == "on");
	else if (key == "autoindex")
		currentLocation->autoindex = (value == "on");
	else if (key == "rewrite") {
		std::istringstream iss(line);
		std::string rewrite;
		iss >> rewrite;
		iss >> rewrite;
		if (!(iss >> rewrite))
			throw std::runtime_error("Error: invalid rewrite value");
		removeCharacter(rewrite, ';');
		removeCharacter(rewrite, '"');
		currentLocation->rewrite = rewrite;
	}
	else if (key == "allow") {
		std::istringstream iss(line);
		std::string allow;
		iss >> allow;
		while (iss >> allow) {
			removeCharacter(allow, ';');
			currentLocation->allow.push_back(allow);
		}
	}
}

void Config::parseServer(std::string key, std::string value, std::string line) {
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
	else if (key == "client_max_body_size") {
		std::istringstream iss(value);
		int size;
		if (!(iss >> size))
			throw std::runtime_error("Error: invalid client_max_body_size");
		this->_tempServer.client_max_body_size = size;
	}
	else if (key == "client_body_in_file_only")
		this->_tempServer.client_body_in_file_only = (value == "on");
	else if (key == "client_body_buffer_size") {
		std::istringstream iss(value);
		int size;
		if (!(iss >> size))
			throw std::runtime_error("Error: invalid client_body_buffer_size");
		this->_tempServer.client_body_buffer_size = size;
	}
	else if (key == "client_body_timeout") {
		std::istringstream iss(value);
		int timeout;
		if (!(iss >> timeout))
			throw std::runtime_error("Error: invalid client_body_timeout");
		this->_tempServer.client_body_timeout = timeout;
	}
	else if (key == "cgi")
		this->_tempServer.cgi = (value == "on");
	else if (key == "cgi_extension")
		this->_tempServer.cgi_extension = value;
	else if (key == "cgi_bin")
		this->_tempServer.cgi_bin = value;
	else if (key == "error_page") {
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
		this->_tempServer.error_pages[error_code] = error_page;
	}
}

bool Config::locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value) {
	if (line.find("}") != std::string::npos && *parsingLocation) {
		this->_tempServer.locations[currentLocation->path] = *currentLocation;
		*parsingLocation = false;
		return (true);
	}
	if (line.find("location") != std::string::npos) {
		if (_tempServer.locations.find(value) != _tempServer.locations.end())
			*currentLocation = _tempServer.locations[value];
		else
			*currentLocation = Location(value);
		*parsingLocation = true;
		return (true);
	}
	return (false);
}

void Config::countBraces(std::string line, int *braceCount) {
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
	makeStatusMessages(_fallBackServer);
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

ServerSettings Config::getServer(std::string serverIP) const { //Throws an exception (std::out_of_range) if the key doesn't exist in the map.
	return this->_Servers.at(serverIP);
}

std::map<std::string, ServerSettings> Config::getServersMap(void) const {
	return this->_Servers;
}

void Config::removeCharacter(std::string& str, char charToRemove) {
	for (std::string::size_type i = 0; i < str.size(); ) {
		if (str[i] == charToRemove)
			str.erase(i, 1);
		else
			++i;
	}
}

void Config::makeStatusMessages(ServerSettings &server) {
	server.error_messages["400"] = "Bad Request";
	server.error_messages["403"] = "Forbidden";
	server.error_messages["404"] = "Not Found";
	server.error_messages["405"] = "Method Not Allowed";
	server.error_messages["413"] = "Request Entity Too Large";
	server.error_messages["415"] = "Unsupported Media Type";
	server.error_messages["500"] = "Internal Server Error";
	server.error_messages["200"] = "OK";
	server.error_messages["201"] = "Created";
	server.error_messages["204"] = "No Content";
}
//################################################ Testing functions ################################################
void Config::printServers(void) const {
	for (std::map<std::string, ServerSettings>::const_iterator serverPair = _Servers.begin(); \
		serverPair != _Servers.end(); ++serverPair) {
		std::cout << serverPair->second << std::endl;
	}
}

void Config::printFallback(void) const {
	std::cout << this->_fallBackServer << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Location& location) {
	os << " Path: " << location.path << std::endl;
	os << " Root: " << location.root << std::endl;
	os << " Index: " << location.index << std::endl;
	os << " Rewrite: " << location.rewrite << std::endl;
	os << " Autoindex: " << (location.autoindex ? "true" : "false") << std::endl;
	os << " Allow:" << std::endl;
	for (std::vector<std::string>::const_iterator it = location.allow.begin(); it != location.allow.end(); ++it) {
		os << "  - " << *it << std::endl;
	}
	os << " Allow Uploads: " << (location.allow_uploads ? "true" : "false") << std::endl;
	os << " Error Pages: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = location.loc_error_pages.begin(); it != location.loc_error_pages.end(); ++it) {
		os << " " << it->first << ": " << it->second << std::endl;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const ServerSettings& server) {
	os << "================ Server ===================" << std::endl;
	os << "Server Name: " << server.server_name << std::endl;
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

	os << "Error Pages:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it) {
		os << " " << it->first << ": " << it->second << std::endl;
	}

	os << "Locations:" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
		os << "============ Location " << it->first << " ==============="<< std::endl << it->second;
	}
	return os;
}
