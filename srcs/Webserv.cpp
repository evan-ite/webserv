#include "../includes/settings.hpp"

/**
 * @brief Default constructor for the Webserv class.
 */
Webserv::Webserv()
{
	for (int i = 0; i < MAX_EVENTS; ++i)
	{
		_fds[i].fd = -1;
		_fds[i].events = 0;
		_fds[i].revents = 0;
		_clientList[i] = NULL;
	}
}

/**
 * @brief Copy constructor for the Webserv class.
 * @param copy The Webserv object to be copied.
 */
Webserv::Webserv(const Webserv &copy) : _servers(copy._servers) {}

/**
 * @brief Destructor for the Webserv class.
 */
Webserv::~Webserv()
{
	for (int i = 0; i < MAX_EVENTS; ++i)
	{
		if (_clientList[i] != NULL)
		{
			delete _clientList[i];
			_clientList[i] = NULL;
		}
	}
}

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
	_nfds = 0;
	std::map<std::string, Server>				sMap = conf.getServersMap();
	std::map<std::string, Server> ::iterator	it = sMap.begin();
	for (; it != sMap.end(); it++)
	{
		it->second.setupServerSocket();
		this->addServer(it->second);
		log(logINFO) << "Server listening: " << it->first << " on fd " << it->second.getFd();
	}

	if (_servers.size() > 0)
		this->handleEvents();
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
	this->pollAdd(s.getFd());
}

/**
 * @brief Adds a file descriptor to the poll listener struct.
 * @param fd The file descriptor to be added.
 */
void Webserv::pollAdd(int fd)
{
	_fds[_nfds].fd = fd;
	_fds[_nfds].events = POLLIN;
	_nfds++;
}

void Webserv::pollRemove(int fd)
{
	for (int i = 0; i < _nfds; i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds[i] = _fds[_nfds - 1];
			_nfds--;
			break;
		}
	}
}

std::vector<Server>::iterator Webserv::findServer(int fd)
{
	std::vector<Server>::iterator it = this->_servers.begin();
	for (; it != this->_servers.end(); it++)
	if (it->getFd() == fd)
		return (it);
	return (it);
}

static struct pollfd ft_accept(int activeFD)
{
	int clientFD = accept(activeFD, NULL, NULL);
	if (clientFD == -1)
	{
		log(logERROR) << "Failed to accept new client connection.";
	}

	makeNonBlocking(clientFD);

	struct pollfd newPollFD;
	newPollFD.fd = clientFD;
	newPollFD.events = POLLIN;
	newPollFD.revents = 0;
	return (newPollFD);
}

void Webserv::handleErrors(int i)
{
	int activeFD = _fds[i].fd;
	std::string err;
	if (_fds[i].revents & POLLERR)
	{
		int error = 0;
		socklen_t errlen = sizeof(error);
		if (getsockopt(activeFD, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
			err = std::string(strerror(error)) + " POLLERR on FD: ";
	}

	if (_fds[i].revents & POLLHUP)
		err = "POLLHUP on FD: ";

	if (_fds[i].revents & POLLNVAL)
		err = "POLLNVAL on FD: ";

	log(logERROR) << err << activeFD;
	delete (_clientList[activeFD]);
	_clientList[activeFD] = NULL;
	this->pollRemove(activeFD);
}

/**
 * Handles the reception of data from a client socket.
 *
 * This function is responsible for accepting new client connections and handling data from existing clients.
 * If the active file descriptor corresponds to a server, a new client connection is accepted and added to the client list.
 * If the active file descriptor corresponds to an existing client, the function calls the receive() method of the client to handle the received data.
 * If the receive() method returns -1, indicating an error, the client is deleted and removed from the client list.
 * If the receive() method returns 1, indicating that the client is ready to send data, the events of the active file descriptor are set to POLLOUT.
 *
 * @param i The index of the active file descriptor in the _fds array.
 */
void Webserv::handleRecv(int i)
{
	std::vector<Server>::iterator serv;
	std::vector<Server>::iterator end = this->_servers.end();
	int activeFD = _fds[i].fd;
	serv = findServer(activeFD);
	if (serv != end)
	{
		// Accept new client connection
		_fds[_nfds] = ft_accept(activeFD);
		int clientFD = _fds[_nfds++].fd;
		Client* c = new Client(*serv, clientFD);
		_clientList[clientFD] = c;

		log(logDEBUG) << "Accepted new client connection, FD: " << clientFD;
	}
	else
	{
		// Handle data from an existing client
		int receiveStatus = _clientList[activeFD]->receive();
		if (receiveStatus == -1)
		{
			delete _clientList[activeFD];
			_clientList[activeFD] = NULL;
			pollRemove(activeFD);
		}
		else if (receiveStatus == 1)
			_fds[i].events = POLLOUT;
	}
}

/**
 * @brief Handles the sending of data to a client.
 *
 * This function is responsible for sending data to a client specified by the file descriptor `i`.
 * It checks if the client exists in the `_clientList` and if not, returns immediately.
 * If the send operation is successful, the function updates the file descriptor events to listen for incoming data (POLLIN)
 * and resets the client state for further requests.
 * If the send operation fails, indicating that the client requested to close the connection, the function logs the event,
 * deletes the client object, sets the client entry in the `_clientList` to NULL, and removes the file descriptor from polling.
 *
 * @param i The index of the file descriptor in the `_fds` array.
 */
void Webserv::handleSend(int i)
{
	int activeFD = _fds[i].fd;
	if (!_clientList[activeFD])
		return ;
	int sendStatus = _clientList[activeFD]->ft_send();
	if (sendStatus == -1)
	{
		delete (_clientList[activeFD]);
		_clientList[activeFD] = NULL;
		pollRemove(activeFD);
	}
	else if (sendStatus == 1)
	{
		_fds[i].events = POLLIN;
		_clientList[activeFD]->resetClient();
	}
}

/**
 * @brief Handles the events for the web server.
 */
void Webserv::handleEvents()
{
	while (g_signal)
	{
		int numEvents = poll(_fds, _nfds, -1);
		if (numEvents == -1)
			throw pollError();

		for (int i = 0; i < _nfds; ++i)
		{
			if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				this->handleErrors(i);

			// Handle incoming connection on the server FD
			else if (_fds[i].revents & POLLIN)
				this->handleRecv(i);

			// Handle outgoing data for the client FD
			else if (_fds[i].revents & POLLOUT)
				this->handleSend(i);
		}
	}
}

/**
 * @brief Returns the error message for a configuration error.
 * @return The error message.
 */
const char * Webserv::configError::what() const throw()
{
	return ("Config Error");
}

/**
 * @brief Returns the error message for an internal error.
 * @return The error message.
 */
const char * Webserv::internalError::what() const throw()
{
	return ("Webserv Error");
}

/**
 * @brief Returns the error message for an epoll error.
 * @return The error message.
 */
const char * Webserv::pollError::what() const throw()
{
	if (!g_signal)
		return ("Signal received - shutting down");
	return ("Poll error");
}
