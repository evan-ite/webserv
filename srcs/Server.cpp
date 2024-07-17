#include "../includes/settings.hpp"

// Constructors
Server::Server() {}

Server::Server(const Server &copy)
{
	this->_activeClients = copy._activeClients;
	this->_address = copy._address;
	this->_fd = copy._fd;
	this->_key = copy._key;
	this->_settings = copy._settings;
}

Server::Server(std::string key, ServerSettings settings)
{
	this->_settings = settings;
	this->_key = key;
	memset(&(this->_address), 0, sizeof(this->_address));
	this->_address.sin_port = htons(this->_settings.port);
	this->_address.sin_family = AF_INET;
	// this->_address.sin_addr.s_addr = inet_addr(this->_settings.host.c_str());
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->setupServerSocket();
}

// Destructor
Server::~Server() {}


// Operators
Server & Server::operator=(const Server &assign)
{
	this->_activeClients = assign._activeClients;
	this->_address = assign._address;
	this->_fd = assign._fd;
	this->_key = assign._key;
	this->_settings = assign._settings;
	return (*this);
}

// Getters / Setters
int Server::getFd() const
{
	return (this->_fd);
}

std::string Server::getKey() const
{
	return (this->_key);
}

const char * Server::clientError::what() const throw()
{
	return "Client did something weird";
}
const char * Server::socketError::what() const throw()
{
	return "Socket did something weird";
}

void Server::setupServerSocket()
{
	this->_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_fd == 0)
		throw socketError();
	const int enable = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw socketError();
	if (!makeNonBlocking(this->_fd))
		throw socketError();
	if (bind(this->_fd, (struct sockaddr *)&(this->_address), sizeof(this->_address)) < 0)
		throw socketError();
	if (listen(this->_fd, 6) < 0)
		throw socketError();
}

void	Server::addClient(Client c)
{
	this->_activeClients.push_back(c);
}

int	Server::getNumberActiveClients()
{
	return (this->_activeClients.size());
}

bool operator==(const Server& lhs, const Server& rhs)
{
	return (lhs.getKey() == rhs.getKey()) && (lhs.getFd() == rhs.getFd());
}
