#include "../includes/settings.hpp"

// Constructors
Cookie::Cookie(std::string sessionId) : _sessionId(sessionId)
{
	this->_timeOut = COOKIE_LIFETIME + std::time(0);
}

Cookie::Cookie(const Cookie &copy)
{
	this->_sessionId = copy._sessionId;
	this->_timeOut = copy._timeOut;
}

// Destructor
Cookie::~Cookie() {}

// Operators
Cookie & Cookie::operator=(const Cookie &assign)
{
	this->_sessionId = assign._sessionId;
	this->_timeOut = assign._timeOut;
	return (*this);
}


// Getters / Setters
const std::string Cookie::getSessionId() const
{
	return (this->_sessionId);
}

bool Cookie::getTimeOut()
{
	if (std::time(0) > this->_timeOut)
		return (0);
	else
		this->_timeOut = COOKIE_LIFETIME + std::time(0);
	return (1);
}
