#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "settings.hpp"

class Response
{
	public:
		// Constructors
		Response(Request &request, Location &loc);
		Response(int	status,
				std::string	reason,
				std::string	type,
				std::string	connection,
				std::string	body);

		// Destructor
		~Response();

		// Operators
		Response & operator=(const Response &assign);
		// setters and getters
		void		setStatus(int status);
		void		setReason(std::string reason);
		void		setType(std::string type);
		void		setBody(std::string body);
		void		setConnection(std::string connection);
		std::string	getConnection();
		std::string	getSessionId();
		// funcs
		std::string	makeResponse();
		bool		handleRedir(std::string redir);
		bool		handleCGI(Request &request);

		class ResponseException : public std::exception {
			private:
				std::string _erCode;
			public:
				ResponseException(const std::string& erCode) : _erCode(erCode) {}
				~ResponseException() throw() {}

				const char* what() const throw()
				{ return _erCode.c_str();}
		};

	private:
		Response(const Response &copy);
		Response();
		void		postMethod(Request &request);
		void		createFiles(Request &request, int &status);
		void		getMethod(Request &request);
		void		deleteMethod(Request &request);
		void		checkMethod(HttpMethod method, Request &request);
		void		createDirlisting(std::string dirPath);
		std::string	loopDir(std::string dirPath);
		std::string	getStatusMessage(std::string errorCode);
		std::string extractFilePath(Request &request);
		bool		isValidRequest(Request &request);
		// To create response
		int			_status;
		std::string	_reason;
		std::string	_type;
		std::size_t	_len;
		std::string	_connection;
		std::string	_sessionId;
		std::string	_body;
		std::string _redir;
		std::string _dirlistTemplate;
		// Utils
		Location 		_loc;
};


#endif
