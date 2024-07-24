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
		const std::string&	getKey() const;
		int					getNumberActiveClients();
		void				addClient(Client c);
		void				setupServerSocket();
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};
		class socketError : public std::exception {
			virtual const char* what() const throw();
		};
		ServerSettings			_settings;
	private:
		Server();
		int						_fd;
		struct sockaddr_in		_address;
		std::string				_key;
		std::vector <Client>	_activeClients;
};

bool operator==(const Server& lhs, const Server& rhs);
#endif
