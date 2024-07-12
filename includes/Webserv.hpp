#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "server.hpp"

class Webserv
{
	public:
		Webserv(Config conf);
		Webserv(std::map<std::string, Config> confs);
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
		std::map<std::string, Config>	_confs;
		int								setupServerSocket(int &server_fd, struct sockaddr_in &address);
		int								setupEpoll(int server_fd, int &epoll_fd);
		void							handleIncomingConnections(int epoll_fd, std::vector< std::pair<int, struct sockaddr_in> > initServers);
		void							handleRequest(int socket_fd, const struct sockaddr_in& client_addr);
		int								makeNonBlocking(int server_fd);
};

#endif
