#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "settings.hpp"

class Webserv
{
	public:
		Webserv(Config &conf);
		~Webserv();
		Webserv & operator=(const Webserv &assign);
		int					run();
		class configError : public std::exception {
			virtual const char* what() const throw();
		};
		class epollError : public std::exception {
			virtual const char* what() const throw();
		};
		class internalError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		void				addServer(Server s);
		int					getEpollFD();
		void				epollAddFD(int fd);
		int					getNumberServers();

		Webserv(const Webserv &copy);
		Webserv();
		Config							_conf;
		int								_epoll_fd;
		std::vector<Server>				_servers;
		void							setupEpoll();
		void							handleEpollEvents();
};

#endif
