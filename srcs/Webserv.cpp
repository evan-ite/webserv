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

/**
 * @brief Runs the main server loop.
 *
 * This function sets up the epoll instance, initializes the servers from the configuration,
 * and starts handling epoll events. If no servers are configured, it logs a message and exits.
 *
 * @return An integer indicating the exit status.
 */
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

/**
 * @brief Adds a server to the server list and registers its file descriptor with epoll.
 *
 * @param s The server to be added.
 */
void	Webserv::addServer(Server s)
{
	this->_servers.push_back(s);
	this->epollAddFD(s.getFd());
}

/**
 * @brief Adds a file descriptor to the epoll instance.
 *
 * This function configures the epoll instance to monitor the specified file descriptor
 * for input events using edge-triggered mode.
 *
 * @param fd The file descriptor to be added.
 * @throws epollError if the epoll_ctl call fails.
 */
void Webserv::epollAddFD(int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(this->getEpollFD(), EPOLL_CTL_ADD, fd, &event) == -1)
		throw epollError();
}

/**
 * @brief Sets up the epoll instance.
 *
 * This function creates an epoll instance and stores its file descriptor.
 *
 * @throws epollError if the epoll_create1 call fails.
 */
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

/**
 * @brief Handles epoll events in the main server loop.
 *
 * This function waits for events on the epoll instance and processes them.
 * It handles new connections and existing client requests.
 *
 * @throws epollError if the epoll_wait call fails.
 */
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
