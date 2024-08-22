#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "settings.hpp"

class Server;
class Request;
class Response;

class Client
{
	public:
		Client(Server &server, int fd);
		~Client();
		// funcs
		int		receive();
		int		ft_send();
		void	resetClient();
		class clientError : public std::exception {
			virtual const char* what() const throw();
		};

	private:
		Server&		_server;
		std::string	_key;
		int			_fd;
		Request*	_req;
		Response*	_resp;
		std::string	_sendbuffer;
};

#endif
