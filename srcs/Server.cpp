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
	return "Client did something weird";
}
const char * Server::socketError::what() const throw()
{
	return "Socket did something weird";
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
	if (listen(this->_fd, 6) < 0)
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

bool Server::checkContentLength(std::string httpRequest)
{
	log(logDEBUG) << httpRequest;
	if (httpRequest.find("\r\n\r\n") == std::string::npos)
		return (1);
	if (httpRequest.find("Content-Length") == std::string::npos)
		return (1);
	int cLen = (atoi((findKey(httpRequest, "Content-Length:", '\n').c_str())) / 1024); // convert to kbyte
	log(logDEBUG) << "cLen " << cLen << " this->_settings.client_max_body_size " << this->_settings.client_max_body_size;
	if (cLen > this->_settings.client_max_body_size)
		return (0);
	return (1);
}

void Server::handleRequest(int fd)
{
	char buffer[BUFFER_SIZE];
	ssize_t count;
	std::string httpRequest;
	log(logINFO)	<< "Server " << this->_settings.host
					<< ":" << this->_settings.port
					<< " is reading from fd: " << fd;
	while ((count = read(fd, buffer, BUFFER_SIZE)) > 0)
	{
		httpRequest.append(buffer, count);
		if (!this->checkContentLength(httpRequest))
		{
			httpRequest = TOOLARGE;
			break ;
		}
	}
	if (count == 0)
	{
		close(fd); // Close on empty request
		log(logERROR) << "Empty request or client disconnected, FD: " << fd;
	}
	else if (!httpRequest.empty())
	{
		// log(logDEBUG) << "\n--- REQUEST ---\n" << httpRequest.substr(0, 1000);
		Request request(httpRequest);
		Response res(request, this->_settings);
		std::string resString = res.makeResponse();
		log(logDEBUG) << "\n--- RESPONSE ---\n" << resString.substr(0, 1000);
		const char *resCStr = resString.data();
		makeNonBlocking(fd);
		ssize_t sent = write(fd, resCStr, resString.size());
		log(logDEBUG) << "SENT " << sent;
		if (sent == -1)
		{
			close(fd); // Close on write error
			log(logERROR) << "Error writing to socket, FD: " << fd;
		}
		if (res.getConnection() == "close")
			close(fd);
	}
}
