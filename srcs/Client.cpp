#include "../includes/settings.hpp"

Client::Client(Server &server, int fd) : _server(server)
{
	_fd = fd;
	_req = NULL;
	_resp = NULL;
}


Client::~Client()
{
	this->resetClient();
	close(_fd);
}

/**
 * @brief Receives data from the client socket and processes it.
 *
 * This function receives data from the client socket and processes it to construct a request object.
 * If the request object is not yet constructed, it creates an initial empty request.
 * The received data is appended to the request buffer.
 * If the received data is empty or an error occurs during receiving, the function returns false.
 * If the received data is successfully appended to the request buffer, the function checks if the URI is not empty.
 * If the URI is not empty, it sets the location of the request by finding the corresponding location in the server.
 * If an exception occurs during the processing of the received data, an error response is sent.
 *
 * @return true if the data is successfully received and processed, false otherwise.
 */
int	Client::receive()
{
	log(logDEBUG) << "Receiving on fd " << _fd;
	if (!_req)
		_req = new Request();
	char buffer[BUFFER_SIZE + 1];
	memset(buffer, 0, BUFFER_SIZE + 1);
	int receivedBytes = recv(_fd, buffer, BUFFER_SIZE, 0);

	if (receivedBytes == -1)
	// needs better handling
		return (-1);
	else if (receivedBytes == 0)
		return (-1);

	try
	{
		bool complete  = _req->appendBuffer(buffer, receivedBytes);
		if (!_req->getUri().empty())
			_req->setLocation(_server.findLocation(_req->getUri()));
		_req->getLocation().setServer(&_server);
		_req->checkLength();
		if (complete) {
			// log(logDEBUG) << "Request complete: \n" << _req->printRequest();
			_req->isValidRequest();
			_resp = new Response(*_req);
			return (1);
		}
	}
	catch(const std::exception& e)
	{
		// send error response
		log(logDEBUG) << "Error in http request";
		_resp = new Response(e.what(), _req->getLocation());
		return (1);
	}
	return (0);
}

/**
 * Sends the response stored in the `_resp` member variable to the client.
 * If the response has not been set, or if the send buffer is empty, this function does nothing.
 * The response is sent in chunks of size `BUFFER_SIZE` or less.
 *
 * @throws Client::clientError if an error occurs while sending the response.
 */
int Client::ft_send()
{
	if (!_resp)
		return (0);
	else if (_sendbuffer.empty())
		_sendbuffer = _resp->getResponse();

	// send response
	int sendSize = std::min(static_cast<int>(_sendbuffer.size()), BUFFER_SIZE);
	int s = send(_fd, _sendbuffer.c_str(), sendSize, MSG_DONTWAIT);
	if (s == -1)
	{
		log(logERROR) << "Client send failed";
		return (-1);
	}
	_sendbuffer.erase(0, sendSize);
	if (_sendbuffer.empty())
	{
		if (_resp->getConnection() == "close")
		{
			log(logDEBUG) << "Connection closed on client request";
			return (-1);
		}
		return (1);
	}
	return (0);
}

void Client::resetClient()
{
	delete (_req);
	delete (_resp);
	_req = NULL;
	_resp = NULL;
	_sendbuffer.clear();
}

const char * Client::clientError::what() const throw()
{
	return ("Client error");
}
