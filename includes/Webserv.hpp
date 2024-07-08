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
		Config	_conf;
		int		setupServerSocket(int &server_fd, struct sockaddr_in &address);
		int		setupEpoll(int server_fd, int &epoll_fd);
		void	handleIncomingConnections(int server_fd, int epoll_fd, struct sockaddr_in &address, int addrlen);
		void	handleRequests(int epoll_fd, std::vector<struct epoll_event> &events);
		int		makeNonBlocking(int server_fd);
};

#endif
