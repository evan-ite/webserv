#include "../includes/settings.hpp"

Location::Location(const std::string& path)
{
	this->_path = path;
	this->cgi = false;
}

Location::Location() : _path("") {}

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


bool Location::findAllow(HttpMethod method)
{
	if (this->allow[method])
		return (true);
	return (false);
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
