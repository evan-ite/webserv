#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "settings.hpp"

class Client
{
	public:
		Client();
		Client(int fd);
		Client(const Client &copy);
		Client & operator=(const Client &assign);
		// Destructor
		~Client();
		// Getters / Setters
		int					getFd() const;
		void				setFd(int fd);
		std::string			getKey() const;
		void				setKey(std::string key);
		void				setAddress(struct sockaddr_in addr);
		class acceptError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		std::string						_key;
		int								_fd;
		struct sockaddr_in				_address;
};

bool operator==(const Client& lhs, const Client& rhs);
#endif
