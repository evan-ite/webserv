#include "../includes/settings.hpp"

// Constructors
Client::Client() {}

Client::Client(const Client &copy)
{
	this->_address = copy._address;
	this->_fd = copy.getFd();
	this->_key = copy.getKey();
}

Client::Client(int active_fd)
{
	socklen_t len = sizeof(this->_address);
	int fd = accept(active_fd, (struct sockaddr*)&(this->_address), &len);
	if (fd < 0)
		throw acceptError();
	if (!makeNonBlocking(fd))
		throw acceptError();
	this->setFd(fd);
}

// Destructor
Client::~Client() {}

// Operators
Client& Client::operator=(const Client& assign)
{
	if (this != &assign)
	{
		this->_key = assign._key;
		this->_fd = assign._fd;
		this->_address = assign._address;
	}
	return (*this);
}

// Getters / Setters

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

const char * Client::acceptError::what() const throw()
{
	return ("Accepting failed");
}

bool operator==(const Client& lhs, const Client& rhs)
{
	return (lhs.getKey() == rhs.getKey()) && (lhs.getFd() == rhs.getFd());
}
