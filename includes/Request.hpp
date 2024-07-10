/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: evan-ite <evan-ite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 17:57:13 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/08 15:40:47 by evan-ite         ###   ########.fr       */
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
		
		std::string	_location;
		HttpMethod	_method;
		std::vector<std::pair<std::string, std::string> > _fileData;
		
	private:
		Request();
		void parse(std::string httpRequest);
		void parseMultipart(std::string httpRequest);
		void printFileData(); 
		
		std::string _host; //mandatory!
		int			_contentLenght;
		std::string _userAgent;
		std::string _connection;
};

#endif
