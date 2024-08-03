#ifndef SERVER_HPP
# define SERVER_HPP
# include "settings.hpp"

class Server : public ASetting
{
	public:
		// Constructors
		Server();
		// Server(std::string host, int port);
		Server(const Server &copy);
		// Destructor
		~Server();
		// Operators
		Server & operator=(const Server &assign);
		// Getters / Setters
		void				setHost(std::string host);
		void				setPort(int port);
		int					getFd() const;
		int					addClient(int fd);
		void				addLocation(Location loc);
		int					getMaxSize(std::string loc);
		std::string			getHost() const;
		std::string			getPort() const;
		// funcs
		bool				clientHasFD(int fd);
		void				handleRequest(int fd);
		void				display() const;
		Location			findLocation(std::string uri);
		bool				locationExists(std::string uri);
		// exceptions
		class clientError : public std::exception
		{
			virtual const char* what() const throw();
		};
		class socketError : public std::exception
		{
			virtual const char* what() const throw();
		};
	private:
		std::string							_port;
		std::string							_host;
		int									_fd;
		struct sockaddr_in					_address;
		std::string							_key;
		std::vector <Client>				_activeClients;
		std::vector <Cookie>				_activeCookies;
		std::map <std::string, Location>	_locations;
		void								setupServerSocket();
		bool								checkContentLength(std::string httpRequest, int fd);
		void								requestTooLarge(int fd);
		void 								checkSession(Request &req);
		void 								addSession(std::string sessionId);
		static void*						handleRequestWrapper(void* arg);
};

std::ostream& operator<<(std::ostream& os, const Server& server);

#endif
