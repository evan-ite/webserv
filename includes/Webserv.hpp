#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "settings.hpp"

class Config;

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
		class pollError : public std::exception {
			virtual const char* what() const throw();
		};
		class internalError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		void							addServer(Server s);
		void							pollAdd(int fd);
		void							pollRemove(int fd);
		std::vector<Server>::iterator	findServer(int fd);

		Webserv(const Webserv &copy);
		struct pollfd		_fds[MAX_EVENTS];
		Client*				_clientList[MAX_EVENTS];
		std::vector<Server>	_servers;
		int					_nfds;
		void				handleEvents();
		void				handleErrors(int i);
		void				handleRecv(int i);
		void				handleSend(int i);
};

#endif
