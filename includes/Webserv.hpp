#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "settings.hpp"

class Webserv
{
	public:
		Webserv();
		~Webserv();
		Webserv & operator=(const Webserv &assign);
		int					run(Config conf);
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
		void				epollAddFD(int fd);

		Webserv(const Webserv &copy);
		int					_epoll_fd;
		std::vector<Server>	_servers;
		void				setupEpoll();
		void				handleEpollEvents();
};

#endif
