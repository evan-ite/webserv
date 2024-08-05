#include "../includes/settings.hpp"
// Constructors
Server::Server() {}

Server::Server(const Server &copy)
{
	this->_activeClients = copy._activeClients;
	this->_address = copy._address;
	this->_fd = copy._fd;
	this->_key = copy._key;
	this->_settings = copy._settings;
}

Server::Server(std::string key, ServerSettings settings)
{
	this->_settings = settings;
	this->_key = key;
	memset(&(this->_address), 0, sizeof(this->_address));
	this->_address.sin_port = htons(this->_settings.port);
	this->_address.sin_family = AF_INET;
	if (this->_settings.host == "localhost")
		this->_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	else
		this->_address.sin_addr.s_addr = inet_addr(this->_settings.host.c_str());
	this->setupServerSocket();
}

// Destructor
Server::~Server() {}


// Operators
Server & Server::operator=(const Server &assign)
{
	this->_activeClients = assign._activeClients;
	this->_address = assign._address;
	this->_fd = assign._fd;
	this->_key = assign._key;
	this->_settings = assign._settings;
	return (*this);
}

// Getters / Setters
int Server::getFd() const
{
	return (this->_fd);
}

const char * Server::clientError::what() const throw()
{
	return "Client Error";
}
const char * Server::socketError::what() const throw()
{
	return "Socket Error";
}

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

int	Server::addClient(int fd)
{
	Client c = Client(fd);
	this->_activeClients.push_back(c);
	return (c.getFd());
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
	int cLen = (atoi((findKey(httpRequest, "Content-Length:", '\n').c_str())) / 1024); // convert to kbyte
	if (cLen > this->_settings.client_max_body_size)
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

void Server::addSession(std::string sessionId)
{
	if (sessionId.empty())
		return ;
	Cookie sesh(sessionId);
	this->_activeCookies.push_back(sesh);
}

// void Server::handleRequest(int fd)
// {
// 	char buffer[BUFFER_SIZE];
// 	ssize_t count;
// 	std::string httpRequest;
// 	log(logINFO)	<< "Server " << this->_settings.host
// 					<< ":" << this->_settings.port
// 					<< " is reading from fd: " << fd;
// 	while ((count = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
// 	{
// 		httpRequest.append(buffer, count);
// 		if (!this->checkContentLength(httpRequest, fd))
// 		{
// 			httpRequest.clear();
// 			log(logDEBUG) << "Shutting downd fd: " << fd;
// 			close(fd);
// 			break ;
// 		}
// 	}
// 	if (count == 0 && httpRequest.empty())
// 	{
// 		close(fd);
// 		log(logERROR) << "Empty request on FD: " << fd << " - connection closed";
// 	}
// 	else if (count == -1)
// 	{
// 		log(logINFO) << httpRequest.length() << " bytes received on fd: " << fd;
// 	}
// 	if (!httpRequest.empty())
// 	{
// 		// log(logDEBUG) << "\n--- REQUEST ---\n" << httpRequest.substr(0, 1000);
// 		Request request(httpRequest);
// 		this->checkSession(request);
// 		Response res(request, this->_settings);
// 		std::string resString = res.makeResponse();
// 		this->addSession(res.getSessionId());
// 		// log(logDEBUG) << "\n--- RESPONSE ---\n" << resString.substr(0, 1000);
// 		const char *resCStr = resString.data();
// 		ssize_t sent = write(fd, resCStr, resString.size());
// 		if (sent == -1)
// 		{
// 			close(fd); // Close on write error
// 			log(logERROR) << "Error writing to socket, FD: " << fd;
// 		}
// 		if (res.getConnection() == "close")
// 		{
// 			close(fd);
// 			log(logINFO) << "connection on fd " << fd << " closed on client request";
// 		}
// 		else
// 		{
// 			log(logINFO) << "connection on fd " << fd << " kept alive";
// 		}
// 	}
// }
void* Server::handleRequestWrapper(void* arg)
{
	std::pair<Server*, int>* args = reinterpret_cast<std::pair<Server*, int>*>(arg);
	Server* server = args->first;
	int fd = args->second;
	delete args;

	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	std::string chunkedBody;
	bool isChunked = false;
	log(logINFO) << "Server is reading from fd: " << fd;

	while ((count = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		httpRequest.append(buffer, count);
		if (httpRequest.find("Transfer-Encoding: chunked") != std::string::npos)
			isChunked = true;
		if (isChunked && httpRequest.find("\r\n\r\n") != std::string::npos)
			handleChunkedRequest(httpRequest, isChunked, chunkedBody);
		else {
			if (!server->checkContentLength(httpRequest, fd))
			{
				httpRequest.clear();
				log(logDEBUG) << "Shutting down fd: " << fd;
				close(fd);
				return (NULL);
			}
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
		Response res(request, server->_settings);
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


void Server::handleChunkedRequest(std::string &httpRequest, bool &isChunked, std::string &chunkedBody)
{
	size_t pos = 0;
	std::string chunkSize;
	std::string chunk;
	// save header in chunkedBody
	if (chunkedBody.empty())
	{
		pos = httpRequest.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			chunkedBody = httpRequest.substr(0, pos + 4);
			httpRequest.erase(0, pos + 4);
		}
	}
	while ((pos = httpRequest.find("\r\n")) != std::string::npos)
	{
		chunkSize = httpRequest.substr(0, pos);
		httpRequest.erase(0, pos + 2);
		if (chunkSize.empty())
		{
			isChunked = false;
			break;
		}
		size_t size = 0;
		std::istringstream iss(chunkSize);
		iss >> std::hex >> size;
		if (iss.fail() || size == 0)
		{
			isChunked = false;
			break;
		}
		if (httpRequest.length() < size)
			break;
		chunk = httpRequest.substr(0, size);
		httpRequest.erase(0, size + 2);
		if (!chunk.empty())
			chunkedBody += chunk;
	}
	if (!isChunked)
	{
		httpRequest = chunkedBody;
		chunkedBody.clear();
	}
}
