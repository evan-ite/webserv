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
	return "Socket or epoll did something weird";
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
		throw socketError();
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
		throw socketError();
	return (1);
}

void Webserv::handleIncomingConnections(int epoll_fd, std::vector< std::pair<int, struct sockaddr_in> > initServers)
{
	struct epoll_event events[MAX_EVENTS];
	while (g_signal)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
			throw socketError();
		// needs all kinds of other error handling
		for (int i = 0; i < num_events; ++i)
		{
			int socket_fd = events[i].data.fd;
			const struct sockaddr_in& client_addr = initServers[socket_fd].second;
			if (events[i].events & EPOLLIN)
				this->handleRequest(socket_fd, client_addr);
		}
	}
}

void Webserv::handleRequest(int socket_fd, const struct sockaddr_in& client_addr)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Reading from socket, FD:" << socket_fd;
	while ((count = read(socket_fd, buffer, BUFFER_SIZE)) > 0)
		httpRequest.append(buffer, count);
	log(logDEBUG) << errno;
	if (!httpRequest.empty())
	{
		log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		// Response res(httpRequest, this->_conf.getConfigData());
		// std::string resString = res.makeResponse();
		// debug only
		const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
		ssize_t sent = write(socket_fd, response, strlen(response));
		if (sent == -1)
		{
			log(logERROR) << "Error writing to socket, FD:" << socket_fd;
			throw socketError();
		}
	}
	else
	{
		log(logERROR) << "Error reading from socket, FD:" << socket_fd;
		throw socketError();
	}
	log(logINFO) << "Handled request from client " << inet_ntoa(client_addr.sin_addr);
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

	std::vector< std::pair<int, struct sockaddr_in> >	initServers;
	std::map<std::string, Config> ::iterator		it;
	int												epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		throw socketError();
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
	this->handleIncomingConnections(epoll_fd, initServers);
		throw socketError();
	for (std::vector<int>::size_type i = 0; i < initServers.size(); ++i)
		close(initServers[i].first);
	close(epoll_fd);
	return (EXIT_SUCCESS);
}
