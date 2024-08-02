#include "../includes/settings.hpp"

Webserv::Webserv() {}

Webserv::Webserv(const Webserv &copy) : _servers(copy._servers) {}

Webserv::~Webserv()
{
	log(logINFO) << "Shutting down webserv";
}

Webserv & Webserv::operator=(const Webserv &assign)
{
	this->_servers = assign._servers;
	return (*this);
}

int	Webserv::run(Config conf)
{
	this->setupEpoll();
	std::map<std::string, Server>				sMap = conf.getServersMap();
	std::map<std::string, Server> ::iterator	it = sMap.begin();
	for (; it != sMap.end(); it++)
	{
		this->addServer(it->second);
		log(logINFO) << "Server listening: " << it->first;
	}

	if (this->_servers.size() > 0)
		this->handleEpollEvents();
	else
		log(logINFO) << "No active server conf found - quitting";
	return (EXIT_SUCCESS);
}

void	Webserv::addServer(Server s)
{
	this->_servers.push_back(s);
	this->epollAddFD(s.getFd());
}

void Webserv::epollAddFD(int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw epollError();
}

void	Webserv::setupEpoll()
{
	this->_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw epollError();
}

void Webserv::handleEpollEvents()
{
	struct epoll_event events[MAX_EVENTS];
	while (g_signal)
	{
		int numEvents = epoll_wait(this->_epoll_fd, events, MAX_EVENTS, -1);
		if (numEvents == -1)
			throw epollError();
		for (int i = 0; i < numEvents; ++i)
		{
			int		activeFD = events[i].data.fd;
			std::vector<Server>::iterator it = this->_servers.begin();
			for (; it != this->_servers.end(); it++)
			{
				if (it->getFd() == activeFD)
				{
					int	clientFD = it->addClient(activeFD);
					this->epollAddFD(clientFD);
					break ; // new connection
				}
				else if (it->clientHasFD(activeFD))
				{
					it->handleRequest(activeFD);
					break ; //old connection
				}
			}
		}
	}
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
