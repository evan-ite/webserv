#include "../includes/settings.hpp"
// Constructors
Server::Server()
{
	this->allow[0] = true;
	this->allow[1] = false;
	this->allow[2] = false;
	this->autoindex = false;
	this->allow_uploads = false;
	this->cgi = false;
}

Server::Server(const Server &copy)
{
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

// Destructor
Server::~Server()
{
	log(logINFO) << "Server " << this->_key << " stopped listening";
}

// Operators
Server & Server::operator=(const Server &assign)
{
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

// Getters / Setters
int Server::getFd() const
{
	return (this->_fd);
}

const char * Server::clientError::what() const throw()
{
	return "Client did something weird";
}
const char * Server::socketError::what() const throw()
{
	return "Socket did something weird";
}

int	Server::addClient(int fd)
{
	Client c = Client(fd);
	this->_activeClients.push_back(c);
	return (c.getFd());
}

void Server::addSession(std::string sessionId)
{
	if (sessionId.empty())
		return ;
	Cookie sesh(sessionId);
	this->_activeCookies.push_back(sesh);
}

void Server::addLocation(Location loc)
{
	this->_locations[loc.getPath()] = loc;
}

void Server::setHost(std::string host)
{
	this->_host = host;
}

void Server::setPort(int port)
{
	this->_port = port;
}

int Server::getMaxSize(std::string loc)
{
	int size;
	try
	{
		size = this->_locations.at(loc).getClientMaxBodySize();
	}
	catch (const std::out_of_range& e)
	{
		size = this->client_max_body_size;
	}
	return (size);
}

void Server::display() const {} // to be implemented?
