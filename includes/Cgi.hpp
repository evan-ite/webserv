#ifndef CGI_HPP
# define CGI_HPP
# include "settings.hpp"

class Cgi
{
	public:
		Cgi(Request &request, Location *loc);
		~Cgi();

		// funcs
		void	execute(Response &response);
		bool	isTrue(); // let's move this out of the class and only call it if we know that it's a CGI request

		class CgiException : public std::exception
		{
			private:
				std::string _erCode;
			public:
				CgiException(const std::string& erCode) : _erCode(erCode) {}
				~CgiException() throw() {}
				const char* what() const throw() {return (_erCode.c_str());}
		};

	private:
		Request 		_request;
		Location		*_loc;
		char			**_env;

		char		**createEnv(std::string const &cgiPath);
		void		executeCgiChild(int *pipefd, std::string cgiScriptPath, std::string interpreter);
		void		executeParent(int pid, int *pipefd, Response &response);
		void		extractCgi(std::string &cgiScriptPath, std::string &interpreter);
		void		createResponse(Response &response, std::string &cgiOutput);
		std::string	readCgiOutput(int *pipefd);

};

#endif
