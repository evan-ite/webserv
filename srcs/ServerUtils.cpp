#include "../includes/settings.hpp"

void Server::setupServerSocket()
{
	this->_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_fd < 0)
		throw socketError();
	const int enable = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw socketError();
	if (!makeNonBlocking(this->_fd))
		throw socketError();
	if (bind(this->_fd, reinterpret_cast<struct sockaddr*>(&_address), sizeof(this->_address)) < 0)
		throw socketError();
	if (listen(this->_fd, 128) < 0)
		throw socketError();
}

bool	Server::clientHasFD(int fd)
{
	std::vector<Client>::iterator it = this->_activeClients.begin();
	for (; it != this->_activeClients.end(); it++)
	{
		if (it->getFd() == fd)
			return (1);
	}
	return (0);
}

bool Server::checkContentLength(std::string httpRequest, int fd)
{
	if (httpRequest.find("\r\n\r\n") == std::string::npos)
		return (1);
	if (httpRequest.find("Content-Length") == std::string::npos)
		return (1);
	size_t firstSpace = httpRequest.find(' ');
	if (firstSpace == std::string::npos)
		return (1);
	size_t secondSpace = httpRequest.find(' ', firstSpace + 1);
	if (firstSpace == std::string::npos)
		return (1);
	std::string location = httpRequest.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	int cLen = (atoi((findKey(httpRequest, "Content-Length:", '\n').c_str())) / 1024); // convert to kbyte
	if (cLen > this->getMaxSize(location))
	{
		this->requestTooLarge(fd);
		return (0);
	}
	return (1);
}

void Server::requestTooLarge(int fd)
{
	Response res(413, "Request object too large", "basic", "close", "");
	std::string response = res.makeResponse();
	const char* resCString = response.c_str();
	ssize_t result = send(fd, resCString, strlen(resCString), 0);
	if (result == -1)
	{
		log(logERROR) << "send error";
	}
}

void Server::checkSession(Request &req)
{
	std::string sessionId = req.getsessionId();
	if (sessionId.empty())
		return ;
	std::vector<Cookie>::iterator it = this->_activeCookies.begin();
	for (; it != this->_activeCookies.end(); it++)
	{
		if (sessionId == it->getSessionId())
		{
			if (it->getTimeOut())
				log(logINFO) << "Session renewed";
			else
			{
				req.resetSessionId();
				it = this->_activeCookies.erase(it);
				log(logINFO) << "Session reset";
			}
			break;
		}
	}

}

void* Server::handleRequestWrapper(void* arg)
{
	std::pair<Server*, int>* args = reinterpret_cast<std::pair<Server*, int>*>(arg);
	Server* server = args->first;
	int fd = args->second;
	delete args;

	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO) << "Server is reading from fd: " << fd;

	while ((count = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		httpRequest.append(buffer, count);
		if (!server->checkContentLength(httpRequest, fd))
		{
			httpRequest.clear();
			log(logDEBUG) << "Shutting down fd: " << fd;
			close(fd);
			return (NULL);
		}
	}

	if (count == 0)
	{
		// Client closed the connection
		if (httpRequest.empty())
		{
			close(fd);
			log(logERROR) << "Empty request on FD: " << fd << " - connection closed";
		}
	}
	// else if (count == -1)
	// {
	// 	if (errno != EINTR)
	// 	{
	// 		log(logERROR) << "Error receiving data on fd: " << fd;
	// 		close(fd);
	// 		return (NULL);
	// 	}
	// 	// Handle other errors or interrupts appropriately
	// }

	if (!httpRequest.empty())
	{
		// log(logDEBUG) << "\n--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		Request request(httpRequest);
		server->checkSession(request);
		Response res(request, server.findLocation(request.getLoc()));
		std::string resString = res.makeResponse();
		server->addSession(res.getSessionId());
		// log(logDEBUG) << "\n--- RESPONSE ---\n" << resString.substr(0, 100);
		const char *resCStr = resString.data();
		ssize_t sent = write(fd, resCStr, resString.size());
		if (sent == -1)
		{
			log(logERROR) << "Error writing to socket, FD: " << fd;
			close(fd);
			return (NULL);
		}

		if (res.getConnection() == "close")
		{
			close(fd);
			log(logINFO) << "Connection on fd " << fd << " closed on client request";
		}
		else
		{
			log(logINFO) << "Connection on fd " << fd << " kept alive";
		}
	}
	return (NULL);
}

void Server::handleRequest(int fd)
{
	pthread_t thread;
	std::pair<Server*, int>* args = new std::pair<Server*, int>(this, fd);
	if (pthread_create(&thread, NULL, this->handleRequestWrapper, args) != 0)
	{
		log(logERROR) << "Failed to create thread for fd: " << fd;
		delete args; // Free the allocated memory on failure
	}
	else
	{
		pthread_detach(thread); // Detach the thread to allow it to run independently
	}
}

void Server::display() const {} // to be implemented?
