#include "../includes/server.hpp"

Webserv::Webserv(Config conf)
{
	this->_conf = conf;
	// this->_req = nullptr;
}

Webserv::Webserv() {}

Webserv::Webserv(const Webserv &copy)
{
	// this->_res = copy._res;
	// this->_req = copy._req;
	this->_conf = copy._conf;
}

Webserv::~Webserv() {}

Webserv & Webserv::operator=(const Webserv &assign)
{
	// this->_res = assign._res;
	// this->_req = assign._req;
	this->_conf = assign._conf;
	return *this;
}

const char * Webserv::configError::what() const throw()
{
	return "Config broken";
}
const char * Webserv::clientError::what() const throw()
{
	return "Client did something weird";
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
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0)
	{
		log(logERROR) << "can't create server fd";
		return (0);
	}
	if (this->makeNonBlocking(server_fd) == -1)
		return (0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->_conf.getConfigData().port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		log(logERROR) << "bind failed";
		close(server_fd);
		return (0);
	}
	if (listen(server_fd, 3) < 0)
	{
		log(logERROR) << "listen failed";
		close(server_fd);
		return (0);
	}
	return (1);
}

int Webserv::setupEpoll(int server_fd, int &epoll_fd)
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		log(logERROR) << "epoll_create1() failed";
		return (0);

	}

	struct epoll_event event;
	event.data.fd = server_fd;
	event.events = EPOLLIN;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
	{
		log(logERROR) << "epoll_ctl() failed";
		close(epoll_fd);
		return (0);

	}
	return (1);

}


void	Webserv::handleIncomingConnections(int server_fd, int epoll_fd, struct sockaddr_in &address, int addrlen)
{
	int								new_socket;
	struct epoll_event				event;
	std::vector<struct epoll_event>	events(MAX_EVENTS);

	while (g_signal)
	{
		int n = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
		for (int i = 0; i < n; i++)
		{
			if (events[i].events & (EPOLLERR | EPOLLHUP) || !(events[i].events & EPOLLIN))
			{
				log(logERROR) << "an error has occurred on this fd: " << server_fd;
				log(logWARNING) << "server continues";
				close(events[i].data.fd);
				continue;
			}
			else if (server_fd == events[i].data.fd)
			{
				log(logINFO) << "new incoming connection";
				while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) != -1)
				{
					if (make_socket_non_blocking(new_socket) == -1)
					{
						close(new_socket);
						continue;
					}
					event.data.fd = new_socket;
					event.events = EPOLLIN | EPOLLET;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1)
					{
						log(logERROR) << "epoll_ctl() error";
						close(new_socket);
						continue;
					}
				}
			}
		}
		this->handleRequests(epoll_fd, events);
	}
}

void Webserv::handleRequests(int epoll_fd, std::vector<struct epoll_event> &events)
{
	for (std::vector<struct epoll_event>::iterator it = events.begin(); it != events.end(); ++it)
	{
		struct epoll_event &event = *it;
		if (event.data.fd != -1 && (event.events & EPOLLIN))
		{
			char buffer[512];
			int count;
			std::string httpRequest;

			log(logINFO) << "data on socket ready to read";

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
				log(logDEBUG) << "--- REQUEST ---\n" << httpRequest;
				Response res(httpRequest, this->_conf.getConfigData());
				std::string resString = res.makeResponse();
				ssize_t sent = write(event.data.fd, resString.c_str(), resString.size());
				if (sent == -1)
				{
					log(logERROR) << "socket write() error: " << strerror(errno);
					close(event.data.fd);
					continue;
				}
				log(logDEBUG) << "--- RESPONSE ---\n" << resString;
				// Optionally modify epoll interest list, e.g., to re-arm the event.
				struct epoll_event ev;
				ev.data.fd = event.data.fd;
				ev.events = EPOLLIN | EPOLLET; // example modification
				if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event.data.fd, &ev) == -1) {
					log(logERROR) << "epoll_ctl() error: " << strerror(errno);
					close(event.data.fd);
				}
			}
		}
	}
}

int	Webserv::run()
{
	int					server_fd;
	int					epoll_fd;
	struct sockaddr_in	address;
	int					addrlen = sizeof(address);

	if (!setupServerSocket(server_fd, address))
		return (EXIT_FAILURE);
	if (!setupEpoll(server_fd, epoll_fd))
		return (EXIT_FAILURE);

	log(logINFO) << "Webserver " << this->_conf.getConfigData().server_name \
	<< " now listening on port " << this->_conf.getConfigData().port;
	this->handleIncomingConnections(server_fd, epoll_fd, address, addrlen);
	close(server_fd);
	close(epoll_fd);
	log(logINFO) << "Webserver " << this->_conf.getConfigData().server_name \
	<< " shutting down";
	return (EXIT_SUCCESS);
}
