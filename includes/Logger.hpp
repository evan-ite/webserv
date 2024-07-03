/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 09:59:14 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/03 10:42:09 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <sstream>

enum loglevel_e
{
	logERROR,
	logWARNING,
	logINFO,
	logDEBUG
};

class Logger
{
public:
	Logger(loglevel_e _loglevel = logERROR);
	~Logger();
	template <typename T>
	Logger& operator<<(T const& value)
	{
		this->_buffer << value;
		return *this;
	}
private:
	std::ostringstream _buffer;
};

extern loglevel_e loglevel;

# define LOGLEVEL logDEBUG
# define log(level) if (level > loglevel) ; else Logger(level)

#endif
