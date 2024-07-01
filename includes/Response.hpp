#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>

class Response
{
	public:
		// Constructors
		Response(std::string status,
				std::string reason,
				std::string type,
				std::string connection,
				std::string body);
		Response(const Response &copy);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);

	private:
		std::string	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;
};

#endif
