#ifndef SERVER_HPP
# define SERVER_HPP
# include "settings.hpp"

class Server
{
	public:
		// Constructors
		Server();
		Server(std::string key, ServerSettings settings);
		Server(const Server &copy);
		// Destructor
		~Server();
		// Operators
		Server & operator=(const Server &assign);
		// Getters / Setters
		int					getFd() const;
		std::string			getKey() const;
		int					getNumberActiveClients();
		void				addClient(Client c);
		void				setupServerSocket();
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};
		class socketError : public std::exception {
			virtual const char* what() const throw();
		};
	private:
		int						_fd;
		struct sockaddr_in		_address;
		std::string				_key;
		ServerSettings			_settings;
		std::vector <Client>	_activeClients;
};

bool operator==(const Server& lhs, const Server& rhs);
#endif
