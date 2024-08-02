#include "../includes/settings.hpp"

Location::Location(const std::string& path)
{
	this->_path = path;
	this->cgi = false;
}

Location::Location() : _path("") {}

void Location::display() const {}

std::string Location::getPath()
{
	return (this->_path);
}

std::string Location::getRedir()
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
