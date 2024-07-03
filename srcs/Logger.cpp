/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 10:01:35 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/03 16:50:34 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

loglevel_e loglevel = LOGLEVEL;

Logger::Logger(loglevel_e _loglevel)
{
	this->_buffer << _loglevel << " : ";
}

Logger::~Logger()
{
	this->_buffer << std::endl;
	std::cerr << this->_buffer.str();
}
