#ifndef SERVER_HPP
# define SERVER_HPP
# include "settings.hpp"

class Server : public ASetting
{
	public:
		// Constructors
		Server();
		// Destructor
		~Server();
		// Operators
		Server& operator=(const ASetting& other);
		// Getters / Setters
		void								setHost(std::string host);
		void								setServerName(std::string sname);
		void								setPort(int port);
		int									getFd() const;
		void								addLocation(Location loc);
		std::string							getHost() const;
		std::string							getServerName() const;
		int									getPort() const;
		std::map <std::string, Location>	getLocations() const;
		// funcs
		Location&	findLocation(const std::string& uri);
		bool		locationExists(std::string uri);
		void		setupServerSocket();

		// exceptions
		class socketError : public std::exception
		{
			virtual const char* what() const throw();
		};
		class LocationError : public std::exception
		{
			virtual const char* what() const throw();
		};
	private:
		int									_port;
		int									_fd;
		std::string							_serverName;
		std::string							_host;
		std::map <std::string, Location>	_locations;
};

std::ostream& operator<<(std::ostream& os, const Server& server);

#endif
