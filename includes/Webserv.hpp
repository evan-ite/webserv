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
		void				addClient(Client c);
		void				removeClient(Client c);
		void				addServer(Server s);
		void				removeServer(Server s);
		std::vector<int>	getActiveFDs();
		int					getEpollFD();
		void				epollAddFD(int fd);
		int					getNumberServers();
		std::string			findServer(int fd);
		ServerSettings		findClient(int fd);

		Webserv(const Webserv &copy);
		Webserv();
		Config							_conf;
		int								_epoll_fd;
		std::vector<Server>				_servers;
		std::vector<Client>				_clients;
		std::vector<int>				_activeFDs;
		void							setupEpoll();
		void							handleEpollEvents();
		void							handleRequest(ServerSettings sett, int client_fd);
};

#endif
