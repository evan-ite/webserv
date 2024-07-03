#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "server.hpp"

class Webserv
{
	public:
		Webserv(Config conf);
		~Webserv();
		Webserv & operator=(const Webserv &assign);
		int	run();
		class configError : public std::exception {
			virtual const char* what() const throw();
		};
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		Webserv(const Webserv &copy);
		Webserv();
		// Response _res;
		// Request _req;
		Config _conf;

};

#endif
