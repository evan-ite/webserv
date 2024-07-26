/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 17:57:13 by jstrozyk          #+#    #+#             */
/*   Updated: 2024/07/26 17:09:06 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "settings.hpp"

enum HttpMethod {
	GET,
	POST,
	DELETE,
	INVALID
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

		std::string 	getLoc();
		std::string		getContentType();
		HttpMethod		getMethod();
		int				getContentLen();
		std::string		getBody();
		std::string		getConnection();

		std::vector<std::pair<std::string, std::string> >	getFileData();


	private:
		Request();
		void parse(std::string httpRequest);
		void printFileData();
		std::string findBoundary(const std::string& httpRequest);
		void parsePart(const std::string& part);
		void parseMultipart(const std::string& httpRequest);

		std::string	_location;
		HttpMethod	_method;
		std::string _body;
		std::string _contentType;
		int			_contentLength;
		std::string _host;
		std::string _userAgent;
		std::string _connection;

		std::vector<std::pair<std::string, std::string> > _fileData;
};

#endif
