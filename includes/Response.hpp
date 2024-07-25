#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "settings.hpp"

class Response
{
	public:
		// Constructors
		Response(Request &request, ServerSettings &confData);
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

		void		postMethod(Request &request);
		void		createFiles(Request &request, int &status);
		void		getMethod(Request &request);
		void		deleteMethod(Request &request);
		Location	findLoc(const std::string& uri, ServerSettings serverData);
		bool		checkMethod(std::string method);
		void		createDirlisting(std::string fileName, std::string dirPath);
		std::string	findError(std::string errorCode);
		std::string	getErrorMessage(std::string errorCode);

		// To create response
		int	_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_date;
		std::string	_connection;
		std::string	_body;

		// Utils
		Location 		*_loc;
		ServerSettings	*_servSet;
};


#endif
