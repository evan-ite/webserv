#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/server.hpp"

class Response
{
	public:
		// Constructors
		Response(const std::string& status,
				const std::string& reason,
				const std::string& type,
				const std::string& connection,
				const std::string& body);
		Response(const Response &copy);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);

		std::string makeResponse();

	private:
		std::string	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;
};

std::string get_date_time();

#endif
