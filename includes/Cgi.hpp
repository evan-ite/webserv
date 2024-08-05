#ifndef CGI_HPP
# define CGI_HPP
# include "settings.hpp"

class Cgi
{
	public:
		// Constructors
		Cgi(Request &request, Location &loc);

		// Destructor
		~Cgi();

		// Operators
		Cgi & operator=(const Cgi &assign);

		void	execute(Response &response);
		bool	isTrue();

		class CgiException : public std::exception {
			private:
				std::string _erCode;
			public:
				CgiException(const std::string& erCode) : _erCode(erCode) {}
				~CgiException() throw() {}

				const char* what() const throw()
				{ return _erCode.c_str();}
		};

	private:
		Cgi();
		Cgi(const Cgi &copy);

		Request 		_request;
		Location		_loc;
		bool			_isTrue;
		char			**_env;

		char		**createEnv(std::string const &cgiPath);
		void		executeCgiChild(int *pipefd, std::string cgiScriptPath, std::string interpreter);
		void		extractCgi(std::string &cgiScriptPath, std::string &interpreter);
		void		createResponse(Response &response, std::string &cgiOutput);
		std::string readCgiOutput(int *pipefd);

};

#endif
