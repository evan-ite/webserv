#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "settings.hpp"

typedef struct s_connection
{
	std::string						key;
	int								fd;
	struct sockaddr_in				address;
	std::vector<int>				c_fds; // c is short for "client", Elise
	std::vector<struct sockaddr_in>	c_address;
} t_conn;

class Webserv
{
	public:
		Webserv(Config &conf);
		// Webserv(std::map<std::string, Server> allServers);
		~Webserv();
		Webserv & operator=(const Webserv &assign);
		int	run();
		class configError : public std::exception {
			virtual const char* what() const throw();
		};
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};
		class socketError : public std::exception {
			virtual const char* what() const throw();
		};
		class epollError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		Webserv(const Webserv &copy);
		Webserv();
		Config							_conf;
		// std::map<std::string, Server>	_allServers;
		int								setupServerSocket(int &server_fd, struct sockaddr_in &address);
		int								setupEpoll(int server_fd, int &epoll_fd);
		void							handleEpollEvents(int epoll_fd, std::vector<t_conn> initServers);
		void							readRequest(ServerSettings serv, int client_fd);
		int								makeNonBlocking(int server_fd);
};

#endif
