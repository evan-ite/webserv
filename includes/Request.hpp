/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 17:57:13 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/01 18:40:24 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "server.hpp"

enum HttpMethod {
	GET,
	POST,
	DELETE
};

class Request
{
	public:
		Request(std::string httpRequest);
		Request(const Request &copy);
		~Request();
		Request & operator=(const Request &assign);
		class invalidMethod : public std::exception
		{
    	public:
        	const char* what() const noexcept override
			{
            	return "Invalid HTTP method.";
			}
        };
	private:
		Request();
		Request(int method, std::string host, int contenLenght, std::string location, std::string userAgent, int connection);
		HttpMethod _method;
		std::string _host;
		int _contenLenght;
		std::string _location;
		std::string _userAgent;
		std::string _connection;
		void parse(std::string httpRequest);
};

#endif
