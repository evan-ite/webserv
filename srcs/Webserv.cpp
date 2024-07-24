#include "../includes/server.hpp"

Webserv::Webserv() {}

Webserv::Webserv(Config &conf) : _conf(conf) {}

Webserv::Webserv(const Webserv &copy)
{
	this->_conf = copy._conf;
}

Webserv::~Webserv() {}

Webserv & Webserv::operator=(const Webserv &assign)
{
	this->_conf = assign._conf;
	return (*this);
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
		throw socketError();
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw socketError();
	if (listen(server_fd, 6) < 0)
		throw socketError();
	return (1);
}

int Webserv::setupEpoll(int server_fd, int &epoll_fd)
{
	struct epoll_event event;
	event.data.fd = server_fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
		throw epollError();
	return (1);
}

void Webserv::handleEpollEvents(int epoll_fd, std::vector<t_conn> initServers)
{
	struct epoll_event events[MAX_EVENTS];
	int client_fd;
	while (g_signal)
	{
		log(logDEBUG) << "Let's epoll wait for events...";
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

			// Check if this is a new connection on any listening socket
			std::vector<t_conn>::iterator it;
			for (it = initServers.begin(); it != initServers.end(); ++it)
			{
				if (it->fd == socket_fd)
				{
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
					if (client_fd >= 0) {
						log(logDEBUG) << "New connection on fd " << client_fd;
						if (this->makeNonBlocking(client_fd) == -1)
							throw socketError();
						struct epoll_event client_event;
						client_event.data.fd = client_fd;
						client_event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
						if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
							log(logERROR) << "epoll_ctl error: " << strerror(errno);
							close(client_fd); // Close client_fd on failure
						}
						else {
							it->c_address.push_back(client_addr);
							it->c_fds.push_back(client_fd);
						}
					} else {
						log(logERROR) << "Accept error: " << strerror(errno);
					}

					break; // Found the matching listening socket, no need to continue
				}
				else if (std::find(it->c_fds.begin(), it->c_fds.end(), socket_fd) != it->c_fds.end()) {
					this->readRequest((this->_conf.getServersMap())[it->key], socket_fd);
					break;
				}

			}
		}
	}
	close(epoll_fd);
}

void Webserv::readRequest(Server serv, int client_fd)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Reading from socket, FD: " << client_fd;
	while ((count = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
		httpRequest.append(buffer, count);
	}
	// if (count == -1)
	// {
	// 	close(client_fd); // Close on read error
	// 	log(logERROR) << "Read error: " << strerror(errno);
	// 	return ;
	// }
	if (!httpRequest.empty())
	{
		log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		Response res(httpRequest, serv);
		std::string resString = res.makeResponse();
		log(logDEBUG) << "--- RESPONSE ---\n" << resString.substr(0, 1000);
		const char *resCStr = resString.data();
		ssize_t sent = write(client_fd, resCStr, resString.size());
		if (sent == -1)
		{
			close(client_fd); // Close on write error
			log(logERROR) << "Error writing to socket, FD: " << client_fd;
		}
	}
	else
	{
		close(client_fd); // Close on empty request
		log(logERROR) << "Empty request or client disconnected, FD: " << client_fd;
	}
}


int	Webserv::run()
{

	std::vector<t_conn>							initServers;
	std::map<std::string, Server> ::iterator	it;
	std::map<std::string, Server>				map = this->_conf.getServersMap();
	int											epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		throw epollError();
	for (it = map.begin(); it != map.end(); it++)
	{
		t_conn conn;
		conn.key = it->first;
		memset(&conn.address, 0, sizeof(conn.address));
		conn.address.sin_port = htons(it->second.port);
		conn.address.sin_family = AF_INET;
		conn.address.sin_addr.s_addr = INADDR_ANY;
		//conn.address.sin_addr.s_addr = inet_addr(it->second.host.c_str());
		this->setupServerSocket(conn.fd, conn.address);
		this->setupEpoll(conn.fd, epoll_fd);
		initServers.push_back(conn);
		log(logINFO) << "Server listening: " << it->first;
	}
	if (initServers.size() > 0)
		this->handleEpollEvents(epoll_fd, initServers);
	for (std::vector<int>::size_type i = 0; i < initServers.size(); ++i)
		close(initServers[i].fd);
	close(epoll_fd);
	return (EXIT_SUCCESS);
}
