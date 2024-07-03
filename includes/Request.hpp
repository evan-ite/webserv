/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: evan-ite <evan-ite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 17:57:13 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/03 13:11:15 by evan-ite         ###   ########.fr       */
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
			const char* what() const throw()
			{
				return "Invalid HTTP method.";
			}
		};
		
		std::string _location;
		
	private:
		Request();
		HttpMethod _method;
		std::string _host; //mandatory!
		int _contentLenght;
		std::string _userAgent;
		std::string _connection;
		void parse(std::string httpRequest);
};

#endif
