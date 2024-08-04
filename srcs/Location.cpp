#include "../includes/settings.hpp"

Location::Location(const std::string& path)
{
	this->_path = path;
	this->allow[0] = false;
	this->allow[1] = false;
	this->allow[2] = false;
	this->allow[3] = false;
	this->allow[4] = false;
}

Location::Location() : _path("")
{
	this->allow[0] = false;
	this->allow[1] = false;
	this->allow[2] = false;
	this->allow[3] = false;
	this->allow[4] = false;
}

Location::Location(const ASetting& other) : ASetting(other)
{
	const Location* derived = dynamic_cast<const Location*>(&other);
	if (derived)
	{
		this->_path = derived->_path;
		this->_redir = derived->_redir;
		this->_server = derived->_server;
	}
	else
		throw std::bad_cast();
}

/**
 * @brief Assignment operator overload for the Location class.
 *
 * This function assigns the values of another ASetting object to the current Location object.
 * It first calls the base class assignment operator to copy the common settings.
 * Then, it checks if the other object is actually a derived Location object using dynamic_cast.
 * If it is, it copies the specific Location settings (path, redirection, server) to the current object.
 * If it is not, it throws a std::bad_cast exception.
 *
 * @param other The ASetting object to be assigned to the current Location object.
 * @return Location& A reference to the current Location object after assignment.
 * @throws std::bad_cast If the other object is not a derived Location object.
 */
Location& Location::operator=(const ASetting& other)
{
	if (this != &other)
	{
		ASetting::operator=(other);
		const Location* derived = dynamic_cast<const Location*>(&other);
		if (derived)
		{
			this->_path = derived->_path;
			this->_redir = derived->_redir;
			this->_server = derived->_server;
		}
		else
			throw std::bad_cast();
	}
	return (*this);
}

std::string Location::getPath() const
{
	return (this->_path);
}

std::string Location::getRedir() const
{
	return (this->_redir);
}

void Location::setPath(std::string p)
{
	this->_path = p;
}

void Location::setRedir(std::string r)
{
	this->_redir = r;
}

void Location::setServer(Server* s)
{
	this->_server = s;
}

bool Location::findAllow(HttpMethod method)
{
	log(logDEBUG) << "Checking if method is allowed in location";
	log(logDEBUG) << "Method: " << method;
	log(logDEBUG) << "Get: " << this->allow[0];
	log(logDEBUG) << "Post: " << this->allow[1];
	log(logDEBUG) << "Delete: " << this->allow[2];
	log(logDEBUG) << "Self-set: " << this->allow[4];

	if (this->allow[4])
		return (this->allow[method]);
	else
	{
		log(logDEBUG) << "Checking if method is allowed in server";
		return (this->_server->findAllow(method));
	}
	return (false);
}

void Location::addAllow(std::string method)
{
	log(logDEBUG) << "Adding allowed method to location";
	log(logDEBUG) << "Method: " << method;
	this->allow[4] = true; // Indicator that this was set location-specific
	if (method == "GET")
		this->allow[0] = true;
	else if (method == "POST")
		this->allow[1] = true;
	else if (method == "DELETE")
		this->allow[2] = true;
	else
		this->allow[3] = true;
}

std::pair<std::string, std::string>	Location::findError(std::string errorCode)
{

	std::pair<std::string, std::string> error;
	try
	{
		error = this->errors[errorCode];
	}
	catch(const std::exception& e)
	{
		error = this->errors["500"];
	}
	return (error);
}

std::pair<std::string, std::string>	Location::findError(const int errorCode)
{
	std::ostringstream oss;
	oss << errorCode;
	std::pair<std::string, std::string> error;
	try
	{
		error = this->errors[oss.str()];
	}
	catch(const std::exception& e)
	{
		error = this->errors["500"];
	}
	return (error);
}

/**
 * @brief Display the location information.
 */
void Location::display() const
{
	log(logINFO) << this;
}
