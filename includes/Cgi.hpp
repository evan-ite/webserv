#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <string>
# include "settings.hpp"

class Cgi
{
	public:
		// Constructors
		Cgi(Request *request, ServerSettings *serverData);

		// Destructor
		~Cgi();

		// Operators
		Cgi & operator=(const Cgi &assign);

		void	execute(Response &response);
		bool	isTrue();

		class CgiException : public std::exception {
			public:
				const char* what() const throw()
				{ return "Error executing CGI script"; }
		};

	private:
		Cgi();
		Cgi(const Cgi &copy);

		Request *_request;
		ServerSettings	*_serverData;
		bool	_isTrue;

		char ** createEnv(std::string const &cgiPath, std::string const &cgiFile);
};

#endif
