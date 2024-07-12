#include "../includes/server.hpp"

Webserv::Webserv(Config conf)
{
	std::map<std::string, Config> singleServer;
	std::stringstream ss;
	ss << conf.getConfigData().port;
	singleServer[conf.getConfigData().host + ":" + ss.str()] = conf;
	log(logDEBUG) << conf.getConfigData().host;
	this->_confs = singleServer;
}

Webserv::Webserv(std::map<std::string, Config> confs) : _confs(confs) {}

Webserv::Webserv() {}

Webserv::Webserv(const Webserv &copy)
{
	this->_confs = copy._confs;
}

Webserv::~Webserv() {}

Webserv & Webserv::operator=(const Webserv &assign)
{
	// this->_res = assign._res;
	// this->_req = assign._req;
	this->_confs = assign._confs;
	return *this;
}

const char * Webserv::configError::what() const throw()
{
	return "Config did something weird";
}
const char * Webserv::clientError::what() const throw()
{
	return "Client did something weird";
}
const char * Webserv::socketError::what() const throw()
{
	return "Socket did something weird";
}
const char * Webserv::epollError::what() const throw()
{
	return "Epoll did something weird";
}

int Webserv::makeNonBlocking(int server_fd)
{
	int flags = fcntl(server_fd, F_GETFL, 0);
	if (flags == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (-1);
	}
	flags |= O_NONBLOCK;
	if (fcntl(server_fd, F_SETFL, flags) == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (-1);
	}
	return 0;
}

int Webserv::setupServerSocket(int &server_fd, struct sockaddr_in &address)
{
	server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server_fd == 0)
		throw socketError();
	const int enable = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw socketError();
	if (this->makeNonBlocking(server_fd) == -1)
<<<<<<< HEAD
		throw socketError();
=======
		return (0);

	// Jan needs to read this https://silviocesare.wordpress.com/2007/10/22/setting-sin_zero-to-0-in-struct-sockaddr_in/
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->_conf.getServer().port);
>>>>>>> Urbano
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw socketError();
	if (listen(server_fd, 6) < 0)
		throw socketError();
	return (1);
}

int Webserv::setupEpoll(int server_fd, int &	epoll_fd)
{
	struct epoll_event event;
	event.data.fd = server_fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
		throw epollError();
	return (1);
}

void Webserv::handleEpollEvents(int epoll_fd, std::vector< std::pair<int, struct sockaddr_in> > initServers)
{
	struct epoll_event events[MAX_EVENTS];
	int client_fd;
	while (g_signal)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
		{
			close(epoll_fd);
			log(logERROR) << "epoll_wait error: " << strerror(errno);
			throw epollError();
		}
		for (int i = 0; i < num_events; ++i)
		{
			int socket_fd = events[i].data.fd;
			bool new_conn = false;

			// Check if this is a new connection on any listening socket
			std::vector< std::pair<int, struct sockaddr_in> >::iterator it;
			for (it = initServers.begin(); it != initServers.end(); ++it)
			{
				if (it->first == socket_fd)
				{
					new_conn = true;
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
					if (client_fd >= 0) {
						log(logDEBUG) << "New connection on fd " << client_fd;
						if (this->makeNonBlocking(client_fd) == -1)
							throw socketError();
						struct epoll_event client_event;
						client_event.data.fd = client_fd;
						client_event.events = EPOLLIN | EPOLLET;
						if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
							log(logERROR) << "epoll_ctl error: " << strerror(errno);
							close(client_fd); // Close client_fd on failure
						}
					} else {
						log(logERROR) << "Accept error: " << strerror(errno);
					}
					break; // Found the matching listening socket, no need to continue
				}
			}

			if (!new_conn) {
				client_fd = socket_fd;
				this->readRequest(client_fd);
			}
		}
	}
	close(epoll_fd);
}

void Webserv::readRequest(int client_fd)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Reading from socket, FD: " << client_fd;

	while ((count = read(client_fd, buffer, BUFFER_SIZE)) > 0)
		httpRequest.append(buffer, count);
	if (count == -1)
	{
		close(client_fd); // Close on read error
		log(logERROR) << "Read error: " << strerror(errno);
		return ;
	}
	if (!httpRequest.empty())
	{
		log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
		ssize_t sent = write(client_fd, response, strlen(response));
		if (sent == -1)
		{
<<<<<<< HEAD
			close(client_fd); // Close on write error
			log(logERROR) << "Error writing to socket, FD: " << client_fd;
=======
			char buffer[5120];
			int count;
			std::string httpRequest;

			log(logINFO) << "data sent, socket ready to read";

			while ((count = read(event.data.fd, buffer, sizeof(buffer))) > 0)
				httpRequest.append(buffer, count);
			// this below needs separate error handling, but we can't check errno from what I see?
			// if (count == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			// {
			// 	log(logERROR) << "socket read() error: " << strerror(errno);
			// 	close(event.data.fd);
			// 	continue;
			// }
			if (!httpRequest.empty())
			{
				log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
				Response res(httpRequest, this->_conf.getServer());
				std::string resString = res.makeResponse();
				ssize_t sent = write(event.data.fd, resString.c_str(), resString.size());
				if (sent == -1)
				{
					log(logERROR) << "socket write() error: " << strerror(errno);
					close(event.data.fd);
					continue;
				}
				log(logDEBUG) << "--- RESPONSE ---\n" << resString.substr(0, 1000);
				// Optionally modify epoll interest list, e.g., to re-arm the event.
				struct epoll_event ev;
				ev.data.fd = event.data.fd;
				ev.events = EPOLLIN | EPOLLET; // example modification
				if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event.data.fd, &ev) == -1) {
					log(logERROR) << "epoll_ctl() error: " << strerror(errno);
					close(event.data.fd);
				}
			}
>>>>>>> Urbano
		}
	}
	else
	{
		close(client_fd); // Close on empty request
		log(logERROR) << "Empty request or client disconnected, FD: " << client_fd;
	}
}




// void Webserv::handleRequests(int epoll_fd, std::vector<struct epoll_event> &events)
// {
// 	for (std::vector<struct epoll_event>::iterator it = events.begin(); it != events.end(); ++it)
// 	{
// 		struct epoll_event &event = *it;
// 		if (event.data.fd != -1 && (event.events & EPOLLIN))
// 		{
// 			char buffer[5120];
// 			int count;
// 			std::string httpRequest;

// 			log(logINFO) << "data sent, socket ready to read";

// 			while ((count = read(event.data.fd, buffer, sizeof(buffer))) > 0)
// 				httpRequest.append(buffer, count);
// 			// this below needs separate error handling, but we can't check errno from what I see?
// 			// if (count == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
// 			// {
// 			// 	log(logERROR) << "socket read() error: " << strerror(errno);
// 			// 	close(event.data.fd);
// 			// 	continue;
// 			// }
// 			if (!httpRequest.empty())
// 			{
// 				log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
// 				Response res(httpRequest, this->_conf.getConfigData());
// 				std::string resString = res.makeResponse();
// 				ssize_t sent = write(event.data.fd, resString.c_str(), resString.size());
// 				if (sent == -1)
// 				{
// 					log(logERROR) << "socket write() error: " << strerror(errno);
// 					close(event.data.fd);
// 					continue;
// 				}
// 				log(logDEBUG) << "--- RESPONSE ---\n" << resString.substr(0, 1000);
// 				// Optionally modify epoll interest list, e.g., to re-arm the event.
// 				struct epoll_event ev;
// 				ev.data.fd = event.data.fd;
// 				ev.events = EPOLLIN | EPOLLET; // example modification
// 				if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event.data.fd, &ev) == -1) {
// 					log(logERROR) << "epoll_ctl() error: " << strerror(errno);
// 					close(event.data.fd);
// 				}
// 			}
// 		}
// 	}
// }

int	Webserv::run()
{

<<<<<<< HEAD
	std::vector< std::pair<int, struct sockaddr_in> >	initServers;
	std::map<std::string, Config> ::iterator		it;
	int												epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		throw epollError();
	for (it = this->_confs.begin(); it != this->_confs.end(); it++)
	{
		int					server_fd;
		struct sockaddr_in	address;
		// Jan needs to read this https://silviocesare.wordpress.com/2007/10/22/setting-sin_zero-to-0-in-struct-sockaddr_in/
		memset(&address, 0, sizeof(address));
		address.sin_port = htons(it->second.getConfigData().port);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		this->setupServerSocket(server_fd, address);
		this->setupEpoll(server_fd, epoll_fd);
		initServers.push_back(std::make_pair(server_fd, address));
		log(logINFO) << "Server listening: " << it->first;
	}
	this->handleEpollEvents(epoll_fd, initServers);
	for (std::vector<int>::size_type i = 0; i < initServers.size(); ++i)
		close(initServers[i].first);
	close(epoll_fd);
=======
	if (!setupServerSocket(server_fd, address))
		return (EXIT_FAILURE);
	if (!setupEpoll(server_fd, epoll_fd))
		return (EXIT_FAILURE);

	log(logINFO) << "Webserver " << this->_conf.getServer().server_name \
	<< " now listening on port " << this->_conf.getServer().port;
	this->handleIncomingConnections(server_fd, epoll_fd, address, addrlen);
	close(server_fd);
	close(epoll_fd);
	log(logINFO) << "Webserver " << this->_conf.getServer().server_name \
	<< " shutting down";
>>>>>>> Urbano
	return (EXIT_SUCCESS);
}
