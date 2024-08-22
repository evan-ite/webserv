#include "../includes/settings.hpp"

/**
 * @brief Default constructor for the Server class.
 */
Server::Server()
{
	_fd = -1;
}

/**
 * @brief Destructor for the Server class.
 */
Server::~Server() {}

/**
 * @brief Assignment operator overload for the Server class.
 *
 * This function assigns the values of another ASetting object to the current Server object.
 * It first calls the base class assignment operator to copy the common settings.
 * Then, it checks if the other object is actually a derived class of Server using dynamic_cast.
 * If it is, it copies the specific Server settings such as port, host, fd, address, key, and locations.
 * If the other object is not a derived class of Server, it throws a std::bad_cast exception.
 *
 * @param other The ASetting object to be assigned to the current Server object.
 * @return Server& A reference to the current Server object after assignment.
 * @throws std::bad_cast if the other object is not a derived class of Server.
 */
Server& Server::operator=(const ASetting& other)
{
	if (this != &other)
	{
		ASetting::operator=(other);
		const Server* derived = dynamic_cast<const Server*>(&other);
		if (derived)
		{
			this->_server = NULL;
			this->_port = derived->_port;
			this->_host = derived->_host;
			this->_fd = derived->_fd;
			this->_locations = derived->_locations;

			std::map<std::string, Location>::iterator it = this->_locations.begin();
			for (; it != this->_locations.end(); it++)
			{
				it->second.setServer(this);
			}
		}
		else
			throw std::bad_cast();
	}
	return (*this);
}

/**
 * @brief Sets up the server socket for the Server class.
 * This function creates a socket, sets socket options, binds the socket to the specified address and port,
 * and starts listening for incoming connections.
 * @throws socketError if any error occurs during socket creation, option setting, binding, or listening.
 */
void Server::setupServerSocket()
{
	struct sockaddr_in addr;
	int opt = 1;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw socketError();

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw socketError();

	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_host.c_str());
	if (_host == "localhost")
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
		throw socketError();

	if (listen(_fd, 10) == -1)
		throw socketError();
}

/**
 * @brief Finds the best matching Location object for a given URI.
 *
 * This function searches through the server's location map to find the most specific
 * location that matches the given URI. It iterates through the map of locations and
 * compares each location's key with the URI. If an exact match is not found, it progressively
 * shortens the URI by removing the last segment (after the last '/') and checks again.
 * If no match is found after all iterations, it returns the default location associated
 * with the root path ("/").
 *
 * @param uriRef The URI for which to find the matching Location.
 * @return A reference to the best matching Location object.
 * @throws std::out_of_range if the root location ("/") is not found in the map.
 */
Location& Server::findLocation(const std::string& uriRef)
{
	Location *loc = &_locations.at("/");
	std::map<std::string, Location>::iterator it = _locations.begin();
	for (; it != _locations.end(); it++)
	{
		std::string uri = uriRef;
		std::string location = it->first;
		while(!uri.empty())
		{
			if (uri == location)
			{
				loc = &it->second;
				break;
			}
			else
				uri = uri.substr(0, uri.find_last_of('/'));
		}
	}
	return (*loc);
}

/**
 * @brief Checks if a given location string exists in the server's locations map.
 *
 * @param locationString The location string to check.
 * @return `true` if the location string exists, `false` otherwise.
 */
bool Server::locationExists(std::string locationString)
{
	if (this->_locations.find(locationString) != this->_locations.end())
		return (true);
	return (false);
}

/**
 * @brief Add a location to the server. Will overwrite without warning.
 * @param loc The location to be added.
 */
void Server::addLocation(Location loc)
{
	_locations[loc.getPath()] = loc;
}

/**
 * @brief Set the host of the server.
 * @param host The host to be set.
 */
void Server::setHost(std::string host)
{
	_host = host;
}

/**
 * @brief Set the port of the server.
 * @param port The port to be set.
 */
void Server::setPort(int port)
{
	_port = port;
}

/**
 * @brief Set the server name.
 * @param sn The server name.
 */
void Server::setServerName(std::string sn)
{
	_serverName = sn;
}

/**
 * @brief Get the host of the server.
 * @return The host of the server.
 */
std::string Server::getHost() const
{
	return(this->_host);
}

/**
 * @brief Get the host of the server.
 * @return The host of the server.
 */
std::string Server::getServerName() const
{
	return(this->_serverName);
}

/**
 * @brief Get the port of the server.
 * @return The port of the server.
 */
int Server::getPort() const
{
	return(this->_port);
}

/**
 * @brief Get the file descriptor of the server.
 * @return The file descriptor of the server.
 */
int Server::getFd() const
{
	return (_fd);
}

/**
 * @brief Get the error message for a socket error.
 * @return The error message for a socket error.
 */
const char* Server::socketError::what() const throw()
{
	return ("Socket Error");
}

/**
 * @brief Get the error message for a Location error.
 * @return The error message for a Location error.
 */
const char* Server::LocationError::what() const throw()
{
	return ("Location Error");
}

/**
 * @brief Get the Location map.
 * @return The Location map.
 */
std::map<std::string, Location> Server::getLocations() const
{
	return (_locations);
}
