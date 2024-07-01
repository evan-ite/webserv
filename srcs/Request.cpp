/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 17:54:58 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/01 19:09:19 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(std::string httpRequest)
{
	this->parse(httpRequest);
}

Request::Request() {}

Request::Request(const Request &copy)
{
	this->_method = copy._method;
	this->_host = copy._host;
	this->_contenLenght = copy._contenLenght;
	this->_location = copy._location;
	this->_userAgent = copy._userAgent;
	this->_connection = copy._connection;
}

Request::Request(int method, std::string host, int contenLenght, std::string location, std::string userAgent, int connection)
{
	this->_method = method;
	this->_host = host;
	this->_contenLenght = contenLenght;
	this->_location = location;
	this->_userAgent = userAgent;
	this->_connection = connection;
}

Request::~Request() {}

Request & Request::operator=(const Request &assign)
{
	this->_method = assign._method;
	this->_host = assign._host;
	this->_contenLenght = assign._contenLenght;
	this->_location = assign._location;
	this->_userAgent = assign._userAgent;
	this->_connection = assign._connection;
	return *this;
}

void Request::parse(std::string httpRequest)
{
	std::string	method = splitReturnFirst(httpRequest, " ");
	if (method == "GET")
	{
		this->_method = GET;
		this->_location = find_key(httpRequest, "GET ", ' ');
	}
	else if (method == "POST")
	{
		this->_method = POST;
		this->_location = find_key(httpRequest, "POST ", ' ');
	}
	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = find_key(httpRequest, "DELETE ", ' ');
	}
	else
		throw Request::invalidMethod();
	this->_userAgent = find_key(httpRequest, "User-Agent:", '\n');
	this->_host = find_key(httpRequest, "Host:", '\n');
	this->_connection = find_key(httpRequest, "Connection:", '\n');
	this->_contenLenght = atoi(find_key(httpRequest, "Content-Length:", '\n').c_str());
}
