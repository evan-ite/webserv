#ifndef SERVER_HPP
# define SERVER_HPP
# include "settings.hpp"

class Server
{
	public:
		// Constructors
		Server(std::string key, ServerSettings settings);
		Server(const Server &copy);
		// Destructor
		~Server();
		// Operators
		Server & operator=(const Server &assign);
		// Getters / Setters
		int					getFd() const;
		bool				clientHasFD(int fd);
		int					addClient(int fd);
		void				handleRequest(int fd);
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};
		class socketError : public std::exception {
			virtual const char* what() const throw();
		};
		void					timeoutLoop();
	private:
		Server();
		const std::string&		getKey() const;
		void					setupServerSocket();
		int						_fd;
		ServerSettings			_settings;
		struct sockaddr_in		_address;
		std::string				_key;
		std::vector <Client>	_activeClients;
		pthread_t				t;
};

bool operator==(const Server& lhs, const Server& rhs);
#endif