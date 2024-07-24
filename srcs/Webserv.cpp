#include "../includes/settings.hpp"

Webserv::Webserv()
{
	memset(this->_clients, 0, sizeof(this->_clients));
}

Webserv::Webserv(Config &conf) : _conf(conf)
{
	memset(this->_clients, 0, sizeof(this->_clients));
}

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
			int activeFD = events[i].data.fd;
			ServerSettings* sett = this->findClient(activeFD);
			log(logDEBUG) << "I LOVE THis\n" << *sett;

			if (sett)
				this->handleRequest(sett, activeFD);
			else if (this->findServer(activeFD))
			{
				std::string servKey = *this->findServer(activeFD);
				Client *c = new Client(servKey, activeFD);
				this->addClient(c);
			}
			else
				throw epollError();
		}
	}
}

void Webserv::handleRequest(ServerSettings* sett, int fd)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Reading from socket, FD: " << fd;
	while ((count = read(fd, buffer, BUFFER_SIZE)) > 0)
		httpRequest.append(buffer, count);
	// if (count == -1)
	// {
	// 	close(fd); // Close on read error
	// 	log(logERROR) << "Read error: " << strerror(errno);
	// 	return ;
	// }
	if (!httpRequest.empty())
	{
		log(logDEBUG) << "\n--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		Response res(httpRequest, sett);
		std::string resString = res.makeResponse();
		log(logDEBUG) << "\n--- RESPONSE ---\n" << resString.substr(0, 1000);
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


const std::string* Webserv::findServer(int fd)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		if (it->getFd() == fd)
			return &(it->getKey());
	}
	log(logERROR) << "Server not found for fd: " << fd;
	return NULL;
}

ServerSettings* Webserv::findClient(int fd)
{
	if (this->_clients[fd] != 0) // error here
	{
		ServerSettings *sett = new ServerSettings;
		log(logDEBUG) << this->_clients[fd]->getKey();
		log(logDEBUG) << (this->_conf.getServersMap().at(this->_clients[fd]->getKey())).host;
		*sett = (this->_conf.getServersMap().at(this->_clients[fd]->getKey()));
		return (sett);
	}
	else
		return (NULL);
}


void	Webserv::addClient(Client* c)
{
	this->_clients[c->getFd()] = c;
	this->epollAddFD(c->getFd());
}


void	Webserv::removeClient(Client c)
{
	this->_clients[c.getFd()] = NULL;
	close(c.getFd());
}

void	Webserv::addServer(Server s)
{
	this->_servers.push_back(s);
	this->epollAddFD(s.getFd());
}

void	Webserv::removeServer(Server s)
{
	close(s.getFd());
	std::vector<Server>::iterator it = this->_servers.begin();
	for (; it != this->_servers.end(); it++)
	{
		if (*it == s)
		{
			this->_servers.erase(it);
			return ;
		}
	}
	log(logERROR) << "Cannot remove server - not found";
	throw internalError();
}

int	Webserv::getEpollFD()
{
	return (this->_epoll_fd);
}
