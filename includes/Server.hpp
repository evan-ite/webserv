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
		class clientError : public std::exception
		{
			virtual const char* what() const throw();
		};
		class socketError : public std::exception
		{
			virtual const char* what() const throw();
		};
	private:
		Server();
		void					setupServerSocket();
		int						_fd;
		ServerSettings			_settings;
		struct sockaddr_in		_address;
		std::string				_key;
		std::vector <Client>	_activeClients;
		std::vector <Cookie>	_activeCookies;
		bool					checkContentLength(std::string httpRequest, int fd, bool& tooLarge);
		void					requestTooLarge(int fd);
		void 					checkSession(Request &req);
		void 					addSession(std::string sessionId);
		static void*			handleRequestWrapper(void* arg);
		static void				handleChunkedRequest(std::string httpRequest, bool &isChunked, std::string &chunkedBody);
};

#endif
