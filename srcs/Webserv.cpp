/**
 * @file Webserv.cpp
 * @brief Implementation file for the Webserv class.
 */

#include "../includes/settings.hpp"

/**
 * @brief Default constructor for the Webserv class.
 */
Webserv::Webserv() {}

/**
 * @brief Copy constructor for the Webserv class.
 * @param copy The Webserv object to be copied.
 */
Webserv::Webserv(const Webserv &copy) : _servers(copy._servers) {}

/**
 * @brief Destructor for the Webserv class.
 */
Webserv::~Webserv() {}

/**
 * @brief Assignment operator for the Webserv class.
 * @param assign The Webserv object to be assigned.
 * @return A reference to the assigned Webserv object.
 */
Webserv & Webserv::operator=(const Webserv &assign)
{
	this->_servers = assign._servers;
	return (*this);
}

/**
 * @brief Runs the web server with the given configuration.
 * @param conf The configuration object.
 * @return The exit status of the web server.
 */
int	Webserv::run(Config conf)
{
	this->setupEpoll();
	std::map<std::string, Server>				sMap = conf.getServersMap();
	std::map<std::string, Server> ::iterator	it = sMap.begin();
	for (; it != sMap.end(); it++)
	{
		it->second.setupServerSocket();
		this->addServer(it->second);
		log(logINFO) << "Server listening: " << it->first;
	}

	if (this->_servers.size() > 0)
		this->handleEpollEvents();
	else
		log(logINFO) << "No active server conf found - quitting";
	return (EXIT_SUCCESS);
}

/**
 * @brief Adds a server to the list of servers.
 * @param s The server to be added.
 */
void	Webserv::addServer(Server s)
{
	this->_servers.push_back(s);
	this->epollAddFD(s.getFd());
}

/**
 * @brief Adds a file descriptor to the epoll instance.
 * @param fd The file descriptor to be added.
 */
void Webserv::epollAddFD(int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
		throw epollError();
}

/**
 * @brief Sets up the epoll instance.
 */
void	Webserv::setupEpoll()
{
	this->_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw epollError();
}

/**
 * @brief Handles the events received by the epoll instance.
 */
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

/**
 * @brief Returns the error message for a configuration error.
 * @return The error message.
 */
const char * Webserv::configError::what() const throw()
{
	return ("Config did something weird");
}

/**
 * @brief Returns the error message for an internal error.
 * @return The error message.
 */
const char * Webserv::internalError::what() const throw()
{
	return ("Webserv did something weird");
}

/**
 * @brief Returns the error message for an epoll error.
 * @return The error message.
 */
const char * Webserv::epollError::what() const throw()
{
	if (g_signal == 0)
		return ("SIGINT received, shutting down");
	return ("Epoll did something weird");
}
