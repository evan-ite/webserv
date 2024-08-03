#include "../includes/settings.hpp"

/**
 * @brief Default constructor for the Server class.
 */
Server::Server()
{
	// Initialize member variables
	this->allow[0] = true;
	this->allow[1] = false;
	this->allow[2] = false;
	this->autoindex = false;
	this->allow_uploads = false;
	this->cgi = false;
}

/**
 * @brief Copy constructor for the Server class.
 * @param copy The Server object to be copied.
 */
Server::Server(const Server &copy)
{
	// Copy member variables
	this->_port = copy._port;
	this->_host = copy._host;
	this->_fd = copy._fd;
	this->_address = copy._address;
	this->_key = copy._key;
	this->_locations = copy._locations;
	this->root = copy.root;
	this->allow[3] = copy.allow[3];
	this->errors = copy.errors;
	this->dirlistTemplate = copy.dirlistTemplate;
	this->cgi = copy.cgi;
	this->cgi_extension = copy.cgi_extension;
	this->cgi_bin = copy.cgi_bin;
	this->cgi_pass = copy.cgi_pass;
	this->index = copy.index;
	this->autoindex = copy.autoindex;
	this->allow_uploads = copy.allow_uploads;
	this->client_max_body_size = copy.client_max_body_size;
}

/**
 * @brief Destructor for the Server class.
 */
Server::~Server() {}

/**
 * @brief Assignment operator for the Server class.
 * @param assign The Server object to be assigned.
 * @return A reference to the assigned Server object.
 */
Server &Server::operator=(const Server &assign)
{
	// Assign member variables
	this->_port = assign._port;
	this->_host = assign._host;
	this->_fd = assign._fd;
	this->_address = assign._address;
	this->_key = assign._key;
	this->_locations = assign._locations;
	this->root = assign.root;
	this->allow[3] = assign.allow[3];
	this->errors = assign.errors;
	this->dirlistTemplate = assign.dirlistTemplate;
	this->cgi = assign.cgi;
	this->cgi_extension = assign.cgi_extension;
	this->cgi_bin = assign.cgi_bin;
	this->cgi_pass = assign.cgi_pass;
	this->index = assign.index;
	this->autoindex = assign.autoindex;
	this->allow_uploads = assign.allow_uploads;
	this->client_max_body_size = assign.client_max_body_size;
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

std::string Server::getHost() const
{
	return(this->_host);
}

std::string Server::getPort() const
{
	return(this->_port);
}

/**
 * @brief Get the error message for a client error.
 * @return The error message for a client error.
 */
const char *Server::clientError::what() const throw()
{
	return "Client did something weird";
}

/**
 * @brief Get the error message for a socket error.
 * @return The error message for a socket error.
 */
const char *Server::socketError::what() const throw()
{
	return "Socket did something weird";
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
 * @brief Add a location to the server.
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
		size = this->client_max_body_size;
	}
	return (size);
}

/**
 * @brief Find the location associated with the given URI.
 * @param uri The URI to find the location for.
 * @return The location associated with the URI.
 */
Location Server::findLocation(std::string uri)
{
	if (uri.empty() || uri == "/")
		return (this->_locations["/"]);
	std::map<std::string, Location>::iterator it = this->_locations.begin();
	for (; it != this->_locations.end(); it++)
	{
		if (it->first == uri)
			return (it->second);
	}
	size_t pos = uri.find_last_of('/');
	if (pos != std::string::npos)
	{
		uri = uri.substr(0, pos);
		return (this->findLocation(uri));
	}
	return (this->_locations["/"]);
}

/**
 * Checks if a given location string exists in the server's locations map.
 *
 * @param locationString The location string to check.
 * @return `true` if the location string exists, `false` otherwise.
 */
bool Server::locationExists(std::string locationString)
{
	if (this->_locations.find(locationString) != this->_locations.end())
		return (true);
	else
		return (false);
}

/**
 * @brief Display the server information.
 */
void Server::display() const
{
	log(logINFO) << this;
}
