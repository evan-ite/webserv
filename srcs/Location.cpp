#include "../includes/settings.hpp"

/**
 * @brief Constructs a Location object with the given path.
 *
 * @param path The path associated with the location.
 */
Location::Location(const std::string& path)
{
	this->_path = path;
	this->_allow[4] = false;
	this->_cgi = -1;
	this->_autoindex = -1;
	this->_client_max_body_size = -1;
}

Location::Location(Server &serv)
{
	this->_server = &serv;
	this->_allow[4] = false;
	this->_cgi = -1;
	this->_autoindex = -1;
	this->_client_max_body_size = -1;
}

/**
 * @brief Default constructor for the Location class.
 * Initializes the _path member variable to an empty string and sets all elements of the allow array to false.
 */
Location::Location() : _path("")
{
	this->_allow[4] = false;
}

/**
 * @brief Copy constructor for the Location class.
 *
 * This constructor initializes the Location object by copying the values from another ASetting object.
 * It first calls the base class copy constructor to copy the common settings.
 * Then, it checks if the other object is actually a derived Location object using dynamic_cast.
 * If it is, it copies the specific Location settings (path, redirection) to the current object.
 * If it is not, it throws a std::bad_cast exception.
 *
 * @param other The ASetting object to be copied.
 * @throws std::bad_cast If the other object is not a derived Location object.
 */
Location::Location(const ASetting& other) : ASetting(other)
{
	const Location* derived = dynamic_cast<const Location*>(&other);
	if (derived)
	{
		this->_path = derived->_path;
		this->_redir = derived->_redir;
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
 * If it is, it copies the specific Location settings (path, redirection) to the current object.
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
		}
		else
			throw std::bad_cast();
	}
	return (*this);
}

/**
 * @brief Getter for the path variable.
 *
 * @return std::string The path associated with the location.
 */
std::string Location::getPath() const
{
	return (this->_path);
}

std::string Location::getServerName() const
{
	return (this->_server->getServerName());
}

Server* Location::getServer() // debuggin only
{
	return (this->_server);
}

/**
 * @brief Getter for the redirection variable.
 *
 * @return std::string The redirection associated with the location.
 */
std::string Location::getRedir() const
{
	return (this->_redir);
}

/**
 * @brief Setter for the path variable.
 *
 * @param p The new path to be set.
 */
void Location::setPath(std::string p)
{
	this->_path = p;
}

/**
 * @brief Setter for the redirection variable.
 *
 * @param r The new redirection to be set.
 */
void Location::setRedir(std::string r)
{
	this->_redir = r;
}

/**
 * @brief Setter for the server variable.
 *
 * @param s Pointer to the Server object.
 */
void Location::setServer(Server* s)
{
	this->_server = s;
}


