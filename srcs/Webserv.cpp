#include "../includes/settings.hpp"

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
	return ("Config did something weird");
}
const char * Webserv::internalError::what() const throw()
{
	return ("Webserv did something weird");
}
const char * Webserv::epollError::what() const throw()
{
	return ("Epoll did something weird");
}

void Webserv::epollAddFD(int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(this->getEpollFD(), EPOLL_CTL_ADD, fd, &event) == -1)
		throw epollError();
}

void Webserv::handleEpollEvents()
{
	struct epoll_event events[MAX_EVENTS];
	while (g_signal)
	{
		int num_events = epoll_wait(this->getEpollFD(), events, MAX_EVENTS, -1);
		if (num_events == -1)
			throw epollError();
		for (int i = 0; i < num_events; ++i)
		{
			int active_fd = events[i].data.fd;
			std::vector<int> activeFDs = this->getActiveFDs();
			log(logDEBUG) << "active fd " << active_fd;
			if (std::find(activeFDs.begin(), activeFDs.end(), active_fd) != activeFDs.end())
			{
				// new conn
				log(logDEBUG) << "asd new " << active_fd;
				Client c(this->findServer(active_fd), active_fd);
				this->addClient(c);
				activeFDs = this->getActiveFDs();
			}
			else
			{
				// old conn
				log(logDEBUG) << "asd old";
				this->handleRequest(this->findClient(active_fd), active_fd);
			}
		}
	}
}

void Webserv::handleRequest(ServerSettings sett, int fd)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Reading from socket, FD: " << fd;
	while ((count = read(fd, buffer, BUFFER_SIZE)) > 0) {
		httpRequest.append(buffer, count);
	}
	if (count == -1)
	{
		close(fd); // Close on read error
		log(logERROR) << "Read error: " << strerror(errno);
		return ;
	}
	if (!httpRequest.empty())
	{
		log(logDEBUG) << "--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		Response res(httpRequest, sett);
		std::string resString = res.makeResponse();
		log(logDEBUG) << "--- RESPONSE ---\n" << resString.substr(0, 1000);
		const char *resCStr = resString.data();
		ssize_t sent = write(fd, resCStr, resString.size());
		if (sent == -1)
		{
			close(fd); // Close on write error
			log(logERROR) << "Error writing to socket, FD: " << fd;
		}
	}
	else
	{
		close(fd); // Close on empty request
		log(logERROR) << "Empty request or client disconnected, FD: " << fd;
	}
}

void	Webserv::setupEpoll()
{
	this->_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw epollError();
}

int	Webserv::getNumberServers()
{
	return (this->_servers.size());
}

int	Webserv::run()
{
	this->setupEpoll();
	std::map<std::string, ServerSettings>				sMap = this->_conf.getServersMap();
	std::map<std::string, ServerSettings> ::iterator	it = sMap.begin();
	for (; it != sMap.end(); it++)
	{
		Server s(it->first, it->second);
		this->addServer(s);
		log(logINFO) << "Server listening: " << it->first;
	}

	if (this->getNumberServers())
		this->handleEpollEvents();
	return (EXIT_SUCCESS);
}


std::string	Webserv::findServer(int fd)
{
	std::vector<Server>::iterator it = this->_servers.begin();
	for (; it != this->_servers.end(); it++)
	{
		log(logDEBUG) << "iterate fd " << it->getFd();
		if (it->getFd() == fd)
			return (it->getKey());
	}
	log(logERROR) << "Server not found for fd: " << fd;
	throw internalError();
}

ServerSettings	Webserv::findClient(int fd)
{
	std::vector<Client>::iterator it = this->_clients.begin();
	for (; it != this->_clients.end(); it++)
	{
		if (it->getFd() == fd)
			return (this->_conf.getServersMap()[it->getKey()]);
	}
	log(logERROR) << "Client not found for fd: " << fd;
	throw internalError();
}


void	Webserv::addClient(Client c)
{
	this->_clients.push_back(c);
	this->_activeFDs.push_back(c.getFd());
	this->epollAddFD(c.getFd());
}

void	Webserv::removeClient(Client c)
{
	int fd;
	std::vector<Client>::iterator it = this->_clients.begin();
	for (; it != this->_clients.end(); it++)
	{
		if (*it == c)
		{
			fd = it->getFd();
			this->_clients.erase(it);
			break ;
		}
	}
	std::vector<int>::iterator fd_it = this->_activeFDs.begin();
	for (; fd_it != this->_activeFDs.end(); it++)
	{
		if (*fd_it == fd)
		{
			this->_activeFDs.erase(fd_it);
			return ;
		}
	}
	log(logERROR) << "Cannot remove client - not found";
	throw internalError();
}

void	Webserv::addServer(Server s)
{
	this->_servers.push_back(s);
	this->_activeFDs.push_back(s.getFd());
	this->epollAddFD(s.getFd());
}

void	Webserv::removeServer(Server s)
{
	int fd;
	std::vector<Server>::iterator it = this->_servers.begin();
	for (; it != this->_servers.end(); it++)
	{
		if (*it == s)
		{
			fd = it->getFd();
			this->_servers.erase(it);
			break ;
		}
	}
	std::vector<int>::iterator fd_it = this->_activeFDs.begin();
	for (; fd_it != this->_activeFDs.end(); it++)
	{
		if (*fd_it == fd)
		{
			this->_activeFDs.erase(fd_it);
			return ;
		}
	}
	log(logERROR) << "Cannot remove server - not found";
	throw internalError();
}

std::vector<int>	Webserv::getActiveFDs()
{
	return (this->_activeFDs);
}

int	Webserv::getEpollFD()
{
	return (this->_epoll_fd);
}
