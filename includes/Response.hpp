#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "server.hpp"

class Response
{
	public:
		// Constructors
		Response(std::string const &httpRequest, ConfigData confData);
		Response(const Response &copy);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);

		std::string makeResponse();

	private:
		Response();
		int	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;
};

std::string getDateTime();

#endif
