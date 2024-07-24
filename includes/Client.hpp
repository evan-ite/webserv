#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "settings.hpp"

class Client
{
	public:
		Client();
		// Client(std::string key, int fd);
		Client(int fd);
		Client(const Client &copy);
		Client & operator=(const Client &assign);
		// Destructor
		~Client();
		// Getters / Setters
		std::time_t			getLastSeen() const;
		void				setLastSeen(std::time_t lastSeen);
		int					getFd() const;
		void				setFd(int fd);
		std::string			getKey() const;
		void				setKey(std::string key);
		void				setAddress(struct sockaddr_in addr);
		void				timeout();
		class acceptError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		std::time_t						_lastSeen;
		std::string						_key;
		int								_fd;
		struct sockaddr_in				_address;
};

bool operator==(const Client& lhs, const Client& rhs);
#endif
