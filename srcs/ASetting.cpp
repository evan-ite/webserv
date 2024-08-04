#include "../includes/settings.hpp"

ASetting::ASetting() {}

/**
 * @brief Copy constructor for ASetting.
 *
 * @param other The ASetting object to be copied.
 */
ASetting::ASetting(const ASetting& other)
{
	this->root = other.root;
	this->allow[0] = other.allow[0];
	this->allow[1] = other.allow[1];
	this->allow[2] = other.allow[2];
	this->allow[3] = other.allow[3];
	this->allow[4] = other.allow[4];
	this->dirlistTemplate = other.dirlistTemplate;
	this->cgi = other.cgi;
	this->cgi_extension = other.cgi_extension;
	this->cgi_bin = other.cgi_bin;
	this->cgi_pass = other.cgi_pass;
	this->index = other.index;
	this->autoindex = other.autoindex;
	this->allow_uploads = other.allow_uploads;
	this->client_max_body_size = other.client_max_body_size;
	this->errors = other.errors;
	this->server = other.server;
}

ASetting::~ASetting() {}

/**
 * @brief Assignment operator for ASetting.
 *
 * @param other The ASetting object to be assigned.
 * @return A reference to the assigned ASetting object.
 */
ASetting& ASetting::operator=(const ASetting& other)
{
	if (this != &other)
	{
		this->root = other.root;
		this->allow[0] = other.allow[0];
		this->allow[1] = other.allow[1];
		this->allow[2] = other.allow[2];
		this->allow[3] = other.allow[3];
		this->allow[4] = other.allow[4];
		this->dirlistTemplate = other.dirlistTemplate;
		this->cgi = other.cgi;
		this->cgi_extension = other.cgi_extension;
		this->cgi_bin = other.cgi_bin;
		this->cgi_pass = other.cgi_pass;
		this->index = other.index;
		this->autoindex = other.autoindex;
		this->allow_uploads = other.allow_uploads;
		this->client_max_body_size = other.client_max_body_size;
		this->errors = other.errors;
		this->server = other.server;
	}
	return (*this);
}

/**
 * @brief Setter for the root directory.
 *
 * @param root The root directory path.
 */
void ASetting::setRoot(const std::string& root)
{
	this->root = root;
}

/**
 * @brief Setter for the directory listing template.
 *
 * @param dirlistTemplate The directory listing template.
 */
void ASetting::setDirlistTemplate(const std::string& dirlistTemplate)
{
	this->dirlistTemplate = dirlistTemplate;
}

/**
 * @brief Setter for the CGI flag.
 *
 * @param cgi The CGI flag value.
 */
void ASetting::setCgi(bool cgi)
{
	this->cgi = cgi;
}

/**
 * @brief Setter for the CGI extension.
 *
 * @param cgi_extension The CGI extension.
 */
void ASetting::setCgiExtension(const std::string& cgi_extension)
{
	this->cgi_extension = cgi_extension;
}

/**
 * @brief Setter for the CGI bin path.
 *
 * @param cgi_bin The CGI bin path.
 */
void ASetting::setCgiBin(const std::string& cgi_bin)
{
	this->cgi_bin = cgi_bin;
}

/**
 * @brief Setter for the CGI pass.
 *
 * @param cgi_pass The CGI pass.
 */
void ASetting::setCgiPass(const std::string& cgi_pass)
{
	this->cgi_pass = cgi_pass;
}

/**
 * @brief Setter for the autoindex flag.
 *
 * @param autoindex The autoindex flag value.
 */
void ASetting::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

/**
 * @brief Setter for the index file.
 *
 * @param index The index file.
 */
void ASetting::setIndex(const std::string& index)
{
	this->index = index;
}

/**
 * @brief Setter for the allow_uploads flag.
 *
 * @param allow_uploads The allow_uploads flag value.
 */
void ASetting::setAllowUploads(bool allow_uploads)
{
	this->allow_uploads = allow_uploads;
}

/**
 * @brief Setter for the client_max_body_size.
 *
 * @param client_max_body_size The client_max_body_size value.
 */
void ASetting::setClientMaxBodySize(int client_max_body_size)
{
	this->client_max_body_size = client_max_body_size;
}

/**
 * @brief Getter for the root directory.
 *
 * @return The root directory path.
 */
std::string ASetting::getRoot() const
{
	if (this->root.empty())
		return (this->server->getRoot());
	return (this->root);
}

/**
 * @brief Getter for the directory listing template.
 *
 * @return The directory listing template.
 */
std::string ASetting::getDirlistTemplate() const
{
	if (this->dirlistTemplate.empty())
		return (this->server->getDirlistTemplate());
	return (this->dirlistTemplate);
}

/**
 * @brief Getter for the CGI flag.
 *
 * @return The CGI flag value.
 */
int ASetting::getCgi() const
{
	if (this->cgi == -1)
		return (this->server->getCgi());
	return (this->cgi);
}

/**
 * @brief Getter for the CGI extension.
 *
 * @return The CGI extension.
 */
std::string ASetting::getCgiExtension() const
{
	if (this->cgi_extension.empty())
		return (this->server->getCgiExtension());
	return (this->cgi_extension);
}

/**
 * @brief Getter for the CGI bin path.
 *
 * @return The CGI bin path.
 */
std::string ASetting::getCgiBin() const
{
	if (this->cgi_bin.empty())
		return (this->server->getCgiBin());
	return (this->cgi_bin);
}

/**
 * @brief Getter for the CGI pass.
 *
 * @return The CGI pass.
 */
std::string ASetting::getCgiPass() const
{
	if (this->cgi_pass.empty())
		return (this->server->getCgiPass());
	return (this->cgi_pass);
}

/**
 * @brief Getter for the index file.
 *
 * @return The index file.
 */
std::string ASetting::getIndex() const
{
	if (this->index.empty())
		return (this->server->getIndex());
	return (this->index);
}

/**
 * @brief Getter for the autoindex flag.
 *
 * @return The autoindex flag value.
 */
int ASetting::getAutoindex() const
{
	if (this->autoindex == -1)
		return (this->server->getAutoindex());
	return (this->autoindex);
}

/**
 * @brief Getter for the allow_uploads flag.
 *
 * @return The allow_uploads flag value.
 */
int ASetting::getAllowUploads() const
{
	if (this->allow_uploads == -1)
		return (this->server->getAllowUploads());
	return (this->allow_uploads);
}

/**
 * @brief Getter for the client_max_body_size.
 *
 * @return The client_max_body_size value.
 */
int ASetting::getClientMaxBodySize() const
{
	if (this->client_max_body_size == -1)
		return (this->server->getClientMaxBodySize());
	return (this->client_max_body_size);
}

/**
 * @brief Adds an error page to the ASetting object.
 *
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
	this->errors[error_code] = error;
}

/**
 * @brief Adds an allowed HTTP method to the ASetting object.
 *
 * @param method The HTTP method to be allowed.
 */
void ASetting::addAllow(std::string method)
{
	this->allow[4] = false; // set to false when called from SERVER, LOCATION will overwrite
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
 * @brief Finds if the given HTTP method is allowed.
 *
 * @param method The HTTP method to be checked.
 * @return True if the method is allowed, false otherwise.
 */
bool ASetting::findAllow(HttpMethod method)
{
	if (this->allow[4])
		return (this->allow[method]);
	else
		return (this->server->findAllow(method));
	return (false);
}
