#include "../includes/settings.hpp"

// Constructors
Client::Client() {}

Client::Client(const Client &copy)
{
	this->_address = copy._address;
	this->_fd = copy.getFd();
	this->_key = copy.getKey();
	this->_lastSeen = copy.getLastSeen();
}

Client::Client(std::string key, int active_fd)
{
	socklen_t len = sizeof(this->_address);
	int fd = accept(active_fd, (struct sockaddr*)&(this->_address), &len);
	log(logDEBUG) << "new client accepting on fd " << fd;
	if (fd > 0)
		throw acceptError();
	if (!makeNonBlocking(fd))
		throw acceptError();
	this->setFd(fd);
	this->setKey(key);
	this->setLastSeen(std::time(NULL));
}

// Destructor
Client::~Client() {}

// Operators
Client& Client::operator=(const Client& assign)
{
	if (this != &assign)
	{
		this->_lastSeen = assign._lastSeen;
		this->_key = assign._key;
		this->_fd = assign._fd;
		this->_address = assign._address;
	}
	return (*this);
}

// Getters / Setters
std::time_t Client::getLastSeen() const
{
	return (this->_lastSeen);
}
void Client::setLastSeen(std::time_t lastSeen)
{
	this->_lastSeen = lastSeen;
}

int Client::getFd() const
{
	return (this->_fd);
}
void Client::setFd(int fd)
{
	this->_fd = fd;
}

std::string Client::getKey() const
{
	return (this->_key);
}
void Client::setKey(std::string key)
{
	this->_key = key;
}

void Client::setAddress(struct sockaddr_in addr)
{
	this->_address = addr;
}

void Client::timeout()
{
	std::time_t now = std::time(NULL);
	if (now - this->getLastSeen() > CONNECTION_TIMEOUT)
	{
		log(logINFO) << "Connection timed out";
		close(this->getFd());
	}
}

const char * Client::acceptError::what() const throw()
{
	return ("Accepting failed");
}

bool operator==(const Client& lhs, const Client& rhs)
{
	return (lhs.getKey() == rhs.getKey()) && (lhs.getFd() == rhs.getFd());
}
