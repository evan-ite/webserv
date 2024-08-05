#include "../includes/settings.hpp"

/**
 * @brief Default constructor for the Server class.
 */
Server::Server()
{
	this->server = this;
	this->_fd = -1;
}

/**
 * @brief Copy constructor for the Server class.
 * @param copy The Server object to be copied.
 */
Server::Server(const ASetting& other) : ASetting(other)
{
	const Server* derived = dynamic_cast<const Server*>(&other);
	if (derived)
	{
		this->server = this;
		this->_port = derived->_port;
		this->_host = derived->_host;
		this->_fd = derived->_fd;
		this->_key = derived->_key;
		this->_locations = derived->_locations;
	}
	else
		throw std::bad_cast();
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
			this->server = this;
			this->_port = derived->_port;
			this->_host = derived->_host;
			this->_fd = derived->_fd;
			this->_key = derived->_key;
			this->_locations = derived->_locations;
		}
		else
			throw std::bad_cast();
	}
	return (*this);
}

/**
 * @brief Get the file descriptor of the server.
 * @return The file descriptor of the server.
 */
int Server::getFd() const
{
	return (this->_fd);
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
 * @brief Get the port of the server.
 * @return The port of the server.
 */
int Server::getPort() const
{
	return(this->_port);
}

/**
 * @brief Get the error message for a client error.
 * @return The error message for a client error.
 */
const char *Server::clientError::what() const throw()
{
	return "Client Error";
}

/**
 * @brief Get the error message for a socket error.
 * @return The error message for a socket error.
 */
const char *Server::socketError::what() const throw()
{
	return "Socket Error";
}

/**
 * @brief Add a client to the server.
 * @param fd The file descriptor of the client.
 * @return The file descriptor of the added client.
 */
int Server::addClient(int fd)
{
	Client c = Client(fd);
	this->_activeClients.push_back(c);
	return (c.getFd());
}

/**
 * @brief Add a session to the server.
 * @param sessionId The session ID to be added.
 */
void Server::addSession(std::string sessionId)
{
	if (sessionId.empty())
		return;
	Cookie sesh(sessionId);
	this->_activeCookies.push_back(sesh);
}

/**
 * @brief Add a location to the server. Will overwrite without warning.
 * @param loc The location to be added.
 */
void Server::addLocation(Location loc)
{
	this->_locations[loc.getPath()] = loc;
}

/**
 * @brief Set the host of the server.
 * @param host The host to be set.
 */
void Server::setHost(std::string host)
{
	this->_host = host;
}

/**
 * @brief Set the port of the server.
 * @param port The port to be set.
 */
void Server::setPort(int port)
{
	this->_port = port;
}

/**
 * @brief Get the maximum size for a given location.
 * @param loc The location to get the maximum size for.
 * @return The maximum size for the given location.
 */
int Server::getMaxSize(std::string loc)
{
	int size;
	try
	{
		size = this->_locations.at(loc).getClientMaxBodySize();
	}
	catch (const std::out_of_range &e)
	{
		size = this->getClientMaxBodySize();
	}
	return (size);
}

/**
 * @brief Find the longest saved location associated with the given URI.
 * @param uri The URI to find the location for.
 * @return The location associated with the URI.
 */
Location Server::findLocation(const std::string& uri) const
{
	std::map<std::string, Location>::const_iterator it = this->_locations.find(uri);
	if (it != this->_locations.end())
	{
		return (it->second);
	}
	// Fallback: Look for the longest matching prefix (e.g., "/path/to/resource" -> "/path/to")
	std::string path = uri;
	while (true) {
		size_t pos = path.find_last_of('/');
		if (pos == std::string::npos) break;

		path = path.substr(0, pos);
		it = this->_locations.find(path);
		if (it != this->_locations.end()) {
			return (it->second);
		}
	}
	// If no match is found, return the root location ("/")
	it = this->_locations.find("/");
	if (it != this->_locations.end())
		return (it->second);

	throw std::runtime_error("Default location not found in server conf.");
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


void Server::printLocations(std::ostream& os)
{
	std::map<std::string, Location>::iterator it = this->_locations.begin();
	for (; it != this->_locations.end(); it++)
	{
		try
		{
			os << it->second;
		}
		catch (std::bad_cast& e)
		{
			os << "Bad cast exception: " << e.what() << std::endl;
		}
	}
}

// void Server::printLocations(std::ostream& os) const
// {
// 	std::map<std::string, Location>::const_iterator it = this->_locations.begin();
// 	for (; it != this->_locations.end(); it++)
// 	{
// 		os << it->second;
// 	}
// }
