#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "server.hpp"

class Response
{
	public:
		// Constructors
		Response(std::string const &httpRequest, Server confData);
		Response(const Response &copy);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);

		std::string makeResponse();

		class ResponseException : public std::exception {
			public:
				const char* what() const throw() 
				{ return "Error creating response"; }
		};

	private:
		Response();
		void	postMethod(Request request, Server serverData);
		void	getMethod(Request request, Server serverData, std::string root, std::string index);
		void	deleteMethod();
		void	cgiMethod(Request request, Server serverData);

		int	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;
};


#endif
