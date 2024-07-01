/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elisevaniterson <elisevaniterson@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:50:35 by elisevanite       #+#    #+#             */
/*   Updated: 2024/07/01 21:10:02 by elisevanite      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

// Constructors
Response::Response(
	std::string status,
	std::string reason,
	std::string type,
	std::string connection,
	std::string body):
	_status(status),
	_reason(reason),
	_type(type),
	_connection(connection),
	_body(body)
{
	_len = _body.length();
	_date = "the date!!";
}

Response::Response(const Response &copy)
{
	(void) copy;
}


// Destructor
Response::~Response()
{
}


// Operators
Response & Response::operator=(const Response &assign)
{
	(void) assign;
	return *this;
}

