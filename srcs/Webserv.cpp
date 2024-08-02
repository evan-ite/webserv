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
	if (epoll_ctl(this->getEpollFD(), EPOLL_CTL_ADD, fd, &event) == -1)
		throw epollError();
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

int	Webserv::getEpollFD()
{
	return (this->_epoll_fd);
}

void Webserv::handleEpollEvents()
{
	struct epoll_event events[MAX_EVENTS];
	while (g_signal)
	{
		int numEvents = epoll_wait(this->getEpollFD(), events, MAX_EVENTS, -1);
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
	return ("Config Error");
}
const char * Webserv::internalError::what() const throw()
{
	return ("Webserv Error");
}
const char * Webserv::epollError::what() const throw()
{
	return ("Epoll Error");
}
