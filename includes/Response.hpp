#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "settings.hpp"

class Response
{
	public:
		// Constructors
		Response(std::string const &httpRequest, ServerSettings* confData);
		Response(int	status,
				std::string	reason,
				std::string	type,
				std::string	connection,
				std::string	body);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);

		std::string makeResponse();
		void		setStatus(int status);
		void		setReason(std::string reason);
		void		setType(std::string type);
		void		setBody(std::string body);
		void		setConnection(std::string connection);

		class ResponseException : public std::exception {
			public:
				const char* what() const throw()
				{ return "Error creating response"; }
		};

	private:
		Response(const Response &copy);
		Response();

		void		postMethod(Request request, ServerSettings* serverData);
		void		getMethod(Request request, ServerSettings* serverData, std::string root, std::string index);
		void		deleteMethod();
		Location	findLoc(const std::string& uri, ServerSettings* serverData);

		int	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;
};


#endif
