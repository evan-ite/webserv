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

int	Webserv::run()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0)
	{
		log(logERROR) << "can't create server fd";
		return(EXIT_FAILURE);
	}

	if (make_socket_non_blocking(server_fd) == -1)
		return(EXIT_FAILURE);

	// do all the config stuff here!
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8484); //read from this.conf but I'm too lazy now

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		log(logERROR) << "bind failed";
		return(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		log(logERROR) << "listen failed";
		return(EXIT_FAILURE);
	}

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		log(logERROR) << "epoll_create1() failed";
		return(EXIT_FAILURE);
	}

	struct epoll_event event;
	event.data.fd = server_fd;
	event.events = EPOLLIN;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
	{
		log(logERROR) << "epoll_ctl() failed";
		return(EXIT_FAILURE);
	}

	std::vector<struct epoll_event> events(MAX_EVENTS);

	log(logINFO) << "Webserver" << this->_conf.getConfigData().server_name << " now listening on port " << this->_conf.getConfigData().port;
	while (g_signal)
	{
		int n = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
		for (int i = 0; i < n; i++) {
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN))
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
						return(EXIT_FAILURE);
					}
				}
			}
			else
			{
				log(logINFO) << "data on socket ready to read";
				char buffer[512];
				int count;
				while ((count = read(events[i].data.fd, buffer, sizeof(buffer))) > 0)
				{
					log(logDEBUG) << buffer;
					Request req(buffer);
					// Response res;
					write(events[i].data.fd, "ACK", 3); // return ACK for debugging, this is where the magic has to happen
				}
				if (count == -1 && errno != EAGAIN)
				{
					log(logERROR) << "socket read() error";
					close(events[i].data.fd);
				}
			}
		}
	}
	close(server_fd);
	log(logINFO) << "Webserver" << this->_conf.getConfigData().server_name << " shutting down";
	return (EXIT_SUCCESS);
}
