#include "../includes/settings.hpp"


void Config::countBraces(std::string line, int *braceCount)
{
	size_t openBraces = std::count(line.begin(), line.end(), '{');
	size_t closeBraces = std::count(line.begin(), line.end(), '}');
	*braceCount += openBraces - closeBraces;
}

bool Config::locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value)
{
	if (line.find("}") != std::string::npos && *parsingLocation)
	{
		this->_tempServer.locations[currentLocation->path] = *currentLocation;
		*parsingLocation = false;
		return (true);
	}
	if (line.find("location") != std::string::npos)
	{
		if (_tempServer.locations.find(value) != _tempServer.locations.end())
			*currentLocation = _tempServer.locations[value];
		else
			*currentLocation = Location(value);
		*parsingLocation = true;
		return (true);
	}
	return (false);
}


std::vector<std::string> Config::getPorts(std::string server)
{
	std::vector<std::string> ports;
	size_t startPos = 0;

	while (true)
	{
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
	return (ports);
}

std::vector<std::string> Config::getHosts(std::string server)
{
	std::vector<std::string> hosts;
	size_t startPos = 0;

	while (true)
	{
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
	return (hosts);
}

void Config::parseLocation(Server *server, Location *currentLocation, std::string key, std::string value, std::string line)
{
	if (key == "root")
		currentLocation->setRoot(value);
	else if (key == "index")
		currentLocation->setIndex(value);
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
		currentLocation->addErrorPage(error_code, error_page);
	}
	else if (key == "allow_uploads")
		currentLocation->setAllowUploads(((value == "on")));
	else if (key == "autoindex")
		currentLocation->setAutoindex(((value == "on")));
	else if (key == "return")
	{
		std::istringstream iss(line);
		std::string redir;
		iss >> redir;
		iss >> redir;
		if (!(iss >> redir))
			throw std::runtime_error("Error: invalid redirection value");
		removeCharacter(redir, ';');
		removeCharacter(redir, '"');
		currentLocation->setRedir(redir);
	}
	else if (key == "allow")
	{
		std::istringstream iss(line);
		std::string allow;
		iss >> allow;
		while (iss >> allow)
		{
			removeCharacter(allow, ';');
			currentLocation->addAllow(allow);
		}
	}
	else if (key == "cgi")
		currentLocation->setCgi(((value == "on")));
	else if (key == "cgi_extension")
		currentLocation->setCgiExtension(value);
	else if (key == "cgi_bin")
		currentLocation->setCgiBin(value);
}

void Config::parseServer(std::string key, std::string value, std::string line)
{
	if (key == "root")
		this->_tempServer.setRoot(value);
	else if (key == "host")
		this->_tempServer.setHost(value);
	else if (key == "listen")
	{
		std::istringstream iss(value);
		int port;
		if (!(iss >> port))
			throw std::runtime_error("Error: invalid port number");
		this->_tempServer.setPort(port);

	}
	else if (key == "client_max_body_size")
	{
		std::istringstream iss(value);
		int size;
		if (!(iss >> size))
			throw std::runtime_error("Error: invalid client_max_body_size");
		this->_tempServer.setClientMaxBodySize(size);
	}
	else if (key == "cgi")
		this->_tempServer.setCgi((value == "on"));
	else if (key == "cgi_extension")
		this->_tempServer.setCgiExtension(value);
	else if (key == "cgi_bin")
		this->_tempServer.setCgiBin(value);
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
		this->_tempServer.addErrorPage(error_code, this->_tempServer.getRoot() + "/" + error_page);
	}
	else if (key == "dir_list")
	{
		std::string path = this->_tempServer.getRoot() + "/" + value;
		this->_tempServer.setDirlistTemplate(path);
	}
}

void Config::removeCharacter(std::string& str, char charToRemove)
{
	for (std::string::size_type i = 0; i < str.size(); )
	{
		if (str[i] == charToRemove)
			str.erase(i, 1);
		else
			++i;
	}
}
