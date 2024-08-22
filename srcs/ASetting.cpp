#include "../includes/settings.hpp"

ASetting::ASetting()
{
	this->_root.clear();
	this->_allow[0] = this->_allow[1] = this->_allow[2] = this->_allow[3] = this->_allow[4] = 0;
	this->_dirlistTemplate.clear();
	this->_cgi = -1;
	this->_cgi_extension.clear();
	this->_cgi_bin.clear();
	this->_cgi_pass.clear();
	this->_index.clear();
	this->_autoindex = -1;
	this->_client_max_body_size = -1;
	this->_server = NULL;
}

/**
 * @brief Copy constructor for ASetting.
 * @param other The ASetting object to be copied.
 */
ASetting::ASetting(const ASetting& other)
{
	this->_root = other._root;
	this->_allow[0] = other._allow[0];
	this->_allow[1] = other._allow[1];
	this->_allow[2] = other._allow[2];
	this->_allow[3] = other._allow[3];
	this->_allow[4] = other._allow[4];
	this->_dirlistTemplate = other._dirlistTemplate;
	this->_cgi = other._cgi;
	this->_cgi_extension = other._cgi_extension;
	this->_cgi_bin = other._cgi_bin;
	this->_cgi_pass = other._cgi_pass;
	this->_index = other._index;
	this->_autoindex = other._autoindex;
	this->_client_max_body_size = other._client_max_body_size;
	this->_errors = other._errors;
}

ASetting::~ASetting() {}

/**
 * @brief Assignment operator for ASetting.
 * @param other The ASetting object to be assigned.
 * @return A reference to the assigned ASetting object.
 */
ASetting& ASetting::operator=(const ASetting& other)
{
	if (this != &other)
	{
		this->_root = other._root;
		this->_allow[0] = other._allow[0];
		this->_allow[1] = other._allow[1];
		this->_allow[2] = other._allow[2];
		this->_allow[3] = other._allow[3];
		this->_allow[4] = other._allow[4];
		this->_dirlistTemplate = other._dirlistTemplate;
		this->_cgi = other._cgi;
		this->_cgi_extension = other._cgi_extension;
		this->_cgi_bin = other._cgi_bin;
		this->_cgi_pass = other._cgi_pass;
		this->_index = other._index;
		this->_autoindex = other._autoindex;
		this->_client_max_body_size = other._client_max_body_size;
		this->_errors = other._errors;
	}
	return (*this);
}

/**
 * @brief Setter for the root directory.
 * @param root The root directory path.
 */
void ASetting::setRoot(const std::string& root)
{
	this->_root = root;
}

/**
 * @brief Setter for the directory listing template.
 * @param dirlistTemplate The directory listing template.
 */
void ASetting::setDirlistTemplate(const std::string& dirlistTemplate)
{
	this->_dirlistTemplate = dirlistTemplate;
}

/**
 * @brief Setter for the CGI flag.
 * @param cgi The CGI flag value.
 */
void ASetting::setCgi(bool cgi)
{
	this->_cgi = cgi;
}

/**
 * @brief Setter for the CGI extension.
 * @param cgi_extension The CGI extension.
 */
void ASetting::setCgiExtension(const std::string& cgi_extension)
{
	this->_cgi_extension = cgi_extension;
}

/**
 * @brief Setter for the CGI bin path.
 * @param cgi_bin The CGI bin path.
 */
void ASetting::setCgiBin(const std::string& cgi_bin)
{
	this->_cgi_bin = cgi_bin;
}

/**
 * @brief Setter for the CGI pass.
 * @param cgi_pass The CGI pass.
 */
void ASetting::setCgiPass(const std::string& cgi_pass)
{
	this->_cgi_pass = cgi_pass;
}

/**
 * @brief Setter for the autoindex flag.
 * @param autoindex The autoindex flag value.
 */
void ASetting::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

/**
 * @brief Setter for the index file.
 * @param index The index file.
 */
void ASetting::setIndex(const std::string& index)
{
	this->_index = index;
}

/**
 * @brief Setter for the client_max_body_size.
 * @param client_max_body_size The client_max_body_size value.
 */
void ASetting::setClientMaxBodySize(int client_max_body_size)
{
	this->_client_max_body_size = client_max_body_size;
}

/**
 * @brief Getter for the root directory.
 * @return The root directory path.
 */
std::string ASetting::getRoot() const
{
	if (this->_root.empty() && this->_server)
		return (this->_server->getRoot());
	return (this->_root);
}

/**
 * @brief Getter for the directory listing template.
 * @return The directory listing template.
 */
std::string ASetting::getDirlistTemplate() const
{
	if (_dirlistTemplate.empty() && _server)
		return (_server->getDirlistTemplate());
	return (getRoot() + "/" + _dirlistTemplate);
}

/**
 * @brief Getter for the CGI flag.
 * @return The CGI flag value.
 */
int ASetting::getCgi() const
{
	if (this->_cgi == -1  && this->_server)
		return (this->_server->getCgi());
	return (this->_cgi);
}

/**
 * @brief Getter for the CGI extension.
 * @return The CGI extension.
 */
std::string ASetting::getCgiExtension() const
{
	if (this->_cgi_extension.empty() && this->_server)
		return (this->_server->getCgiExtension());
	return (this->_cgi_extension);
}

/**
 * @brief Getter for the CGI bin path.
 * @return The CGI bin path.
 */
std::string ASetting::getCgiBin() const
{
	if (this->_cgi_bin.empty()  && this->_server)
		return (this->_server->getCgiBin());
	return (this->_cgi_bin);
}

/**
 * @brief Getter for the CGI pass.
 * @return The CGI pass.
 */
std::string ASetting::getCgiPass() const
{
	if (this->_cgi_pass.empty()  && this->_server)
		return (this->_server->getCgiPass());
	return (this->_cgi_pass);
}

/**
 * @brief Getter for the index file.
 * @return The index file.
 */
std::string ASetting::getIndex() const
{
	if (this->_index.empty()  && this->_server)
		return (this->_server->getIndex());
	return (this->_index);
}

/**
 * @brief Getter for the autoindex flag.
 * @return The autoindex flag value.
 */
int ASetting::getAutoindex() const
{
	if (this->_autoindex == -1  && this->_server)
		return (this->_server->getAutoindex());
	return (this->_autoindex);
}

/**
 * @brief Getter for the client_max_body_size.
 * @return The client_max_body_size value.
 */
int ASetting::getClientMaxBodySize() const
{
	if (this->_client_max_body_size == -1 && this->_server != NULL)
		return (this->_server->getClientMaxBodySize());
	return (this->_client_max_body_size);
}

/**
 * @brief Adds an error page to the ASetting object.
 * @param error_code The error code.
 * @param error_page The error page.
 */
void ASetting::addErrorPage(std::string error_code, std::string error_page)
{
	std::pair<std::string, std::string> error;
	error.second = error_page;
	switch (atoi(error_code.c_str()))
	{
		case 400:
			error.first = E400;
			break;
		case 403:
			error.first = E403;
			break;
		case 404:
			error.first = E404;
			break;
		case 405:
			error.first = E405;
			break;
		case 413:
			error.first = E413;
			break;
		case 415:
			error.first = E415;
			break;
		case 200:
			error.first = E200;
			break;
		case 201:
			error.first = E201;
			break;
		case 204:
			error.first = E204;
			break;
		default:
			error.first = E500;
	}
	this->_errors[error_code] = error;
}

/**
 * @brief Adds an allowed HTTP method to the ASetting object.
 * @param method The HTTP method to be allowed.
 */
void ASetting::addAllow(std::string method)
{
	this->_allow[4] = true; // set to false when called from SERVER, LOCATION will overwrite
	if (method == "GET")
		this->_allow[0] = true;
	else if (method == "POST")
		this->_allow[1] = true;
	else if (method == "DELETE")
		this->_allow[2] = true;
	else
		this->_allow[3] = true;
}

/**
 * @brief Finds if the given HTTP method is allowed.
 * @param method The HTTP method to be checked.
 * @return True if the method is allowed, false otherwise.
 */
bool ASetting::findAllow(HttpMethod method)
{
	if (this->_allow[4])
		return (this->_allow[method]);
	else if (this->_server)
	{
		log(logDEBUG) << "allow from server";
		return (this->_server->findAllow(method));
	}
	else
		return (0);
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
std::pair<std::string, std::string> ASetting::findError(std::string errorCode)
{
	std::pair<std::string, std::string> error;

	if (_errors.size() && _errors.find(errorCode) != _errors.end())
	{
		error = _errors[errorCode];
		error.second = _root + "/" + error.second;
	}
	else if (this->_server)
		error = _server->findError(errorCode);

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
std::pair<std::string, std::string> ASetting::findError(const int errorCode)
{
	std::pair<std::string, std::string> error;

	if (_errors.size() && _errors.find(toString(errorCode)) != _errors.end())
	{
		error = _errors[toString(errorCode)];
		error.second = _root + "/" + error.second;
	}
	else if (this->_server)
		error = _server->findError(toString(errorCode));

	return (error);
}


void ASetting::print(std::ostream& os) const
{
	os << "Root: " << _root << "\n";
	os << "Allow GET: " << (_allow[0] ? "true" : "false") << "\n";
	os << "Allow POST: " << (_allow[1] ? "true" : "false") << "\n";
	os << "Allow DELETE: " << (_allow[2] ? "true" : "false") << "\n";
	os << "Directory Listing Template: " << _dirlistTemplate << "\n";
	os << "CGI Enabled: " << (_cgi ? "true" : "false") << "\n";
	os << "CGI Extension: " << _cgi_extension << "\n";
	os << "CGI Bin: " << _cgi_bin << "\n";
	os << "CGI Pass: " << _cgi_pass << "\n";
	os << "Index: " << _index << "\n";
	os << "Autoindex: " << (_autoindex ? "true" : "false") << "\n";
	os << "Client Max Body Size: " << _client_max_body_size << "\n";
	os << "Error Pages: \n";
	for (std::map<std::string, std::pair<std::string, std::string> >::const_iterator it = _errors.begin(); it != _errors.end(); ++it)
		os << "  " << it->first << ": " << it->second.second << "\n";
}
