#include "../includes/settings.hpp"

// Setters
void ASetting::setRoot(const std::string& root)
{
	this->root = root;
}

void ASetting::setDirlistTemplate(const std::string& dirlistTemplate)
{
	this->dirlistTemplate = dirlistTemplate;
}

void ASetting::setCgi(bool cgi)
{
	this->cgi = cgi;
}

void ASetting::setCgiExtension(const std::string& cgi_extension)
{
	this->cgi_extension = cgi_extension;
}

void ASetting::setCgiBin(const std::string& cgi_bin)
{
	this->cgi_bin = cgi_bin;
}

void ASetting::setCgiPass(const std::string& cgi_pass)
{
	this->cgi_pass = cgi_pass;
}

void ASetting::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

void ASetting::setIndex(const std::string& index)
{
	this->index = index;
}

void ASetting::setAllowUploads(bool allow_uploads)
{
	this->allow_uploads = allow_uploads;
}

void ASetting::setClientMaxBodySize(int client_max_body_size)
{
	this->client_max_body_size = client_max_body_size;
}

// Getters
std::string ASetting::getRoot() const
{
	return (this->root);
}

std::string ASetting::getDirlistTemplate() const
{
	return (this->dirlistTemplate);
}

bool ASetting::getCgi() const
{
	return (this->cgi);
}

std::string ASetting::getCgiExtension() const
{
	return (this->cgi_extension);
}

std::string ASetting::getCgiBin() const
{
	return (this->cgi_bin);
}

std::string ASetting::getCgiPass() const
{
	return (this->cgi_pass);
}

std::string ASetting::getIndex() const
{
	return (this->index);
}

bool ASetting::getAutoindex() const
{
	return (this->autoindex);
}

bool ASetting::getAllowUploads() const
{
	return (this->allow_uploads);
}

int ASetting::getClientMaxBodySize() const
{
	return (this->client_max_body_size);
}

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

void ASetting::addAllow(std::string method)
{
	if (method == "GET")
		this->allow[0] = true;
	else if (method == "POST")
		this->allow[1] = true;
	else if (method == "DELETE")
		this->allow[3] = true;
	else
		this->allow[4] = true;
}
