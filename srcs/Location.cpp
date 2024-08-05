#include "../includes/settings.hpp"

/**
 * @brief Constructs a Location object with the given path.
 *
 * @param path The path associated with the location.
 */
Location::Location(const std::string& path)
{
	this->_path = path;
	// this->allow[4] = false;
	this->allow[0] = this->allow[1] = this->allow[2] = this->allow[3] = this->allow[4] = 0;
	this->cgi = -1;
	this->autoindex = -1;
	this->allow_uploads = -1;
	this->client_max_body_size = -1;
}

/**
 * @brief Default constructor for the Location class.
 * Initializes the _path member variable to an empty string and sets all elements of the allow array to false.
 */
Location::Location() : _path("")
{
	this->allow[4] = false;
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
	this->server = s;
}

/**
 * @brief Adds an allowed method to the allow array.
 *
 * This function sets the corresponding index in the allow array to true based on the given method.
 * If the method is not recognized, it sets the last index to true as an indicator that this was set location-specific.
 *
 * @param method The method to be added to the allow array.
 */
void Location::addAllow(std::string method)
{
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

/**
 * @brief Finds the error message for the given error code.
 *
 * This function searches for the error message associated with the given error code in the errors map.
 * If the error code is not found, it returns the error message for the "500" error code.
 *
 * @param errorCode The error code to search for.
 * @return std::pair<std::string, std::string> The error message and description.
 */
std::pair<std::string, std::string> Location::findError(std::string errorCode)
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

/**
 * @brief Finds the error message for the given error code.
 *
 * This function searches for the error message associated with the given error code in the errors map.
 * If the error code is not found, it returns the error message for the "500" error code.
 *
 * @param errorCode The error code to search for.
 * @return std::pair<std::string, std::string> The error message and description.
 */
std::pair<std::string, std::string> Location::findError(const int errorCode)
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
