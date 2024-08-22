#include "../includes/settings.hpp"

Response::Response(): _loc(NULL) {}

Response::~Response() {}

/**
 * @brief Constructs a Response object for handling errors.
 *
 * This constructor initializes a Response object using the provided error status and location.
 *
 * @param errStatus The error status code as a string.
 * @param loc The Location object used to find error details.
 */
Response::Response(const char *errStatus, Location &loc)
{
	_loc = &loc;
	_status = atoi(errStatus);
}

/**
 * @brief Constructs a Response object based on the given Request and Location.
 *
 * Initializes a Response object by processing the provided Request and Location.
 * Handles redirection, CGI execution or GET, POST and DELETE methods.
 * On exception, creates error response.
 *
 * @param request The Request object containing the client's request details.
 * @param loc The Location object used to find configuration and error details.
 */
Response::Response(Request &request)
{
	try
	{
		HttpMethod method = request.getMethod();
		_uri = request.getUri();
		_loc = &request.getLocation();
		_status = 200;
		_reason = "OK";
		_contentType = "text/html";
		_connection = request.getConnection();
		if (request.getSessionId().empty())
				this->_sessionId = generateRandomString(12);
		if (this->handleRedir(_loc->getRedir()))
			return ;
		else if (this->handleCGI(request))
			return ;
		this->checkMethod(method, request);
		log(logDEBUG) << "Response object succesfully created";
	}
	catch(std::exception& e)
	{
		_status = atoi(e.what());
	}
}

/**
 * @brief Checks if the given HTTP method is allowed and calls the corresponding method.
 *
 * This function checks the HTTP method of a request.
 * If the method is allowed, it calls the corresponding handler function.
 * If the method is not allowed, it throws a ResponseException with a "405" status code.
 *
 * @param method The HTTP method to check (GET, POST, DELETE).
 * @param request The request object to be processed.
 * @throws ResponseException if the method is not allowed.
 */
void Response::checkMethod(HttpMethod method, Request &request)
{
	void (Response::*funcs[3])(Request &) = {&Response::getMethod, &Response::postMethod, &Response::deleteMethod};

	if (_loc->findAllow(method))
		(this->*funcs[method])(request);
	else
		throw ResponseException("405");
}

/**
 * @brief Handles the HTTP POST method by creating files from the request data.
 *
 * This function processes an HTTP POST request by extracting file data from the request
 * and creating the specified files in the directory determined by the location's root
 * and path. If no file data is provided in the request, it logs an error and throws a
 * ResponseException with a "400" status code. Otherwise, it calls the createFiles function
 * to create the files.
 *
 * @param request The HTTP request containing the file data to be processed.
 * @throws ResponseException if no file data is provided or if file creation fails.
 */
void	Response::postMethod(Request &request)
{
	(void)request;
	std::string file = _loc->getRoot() + "/";
	std::vector<std::pair<std::string, std::string> > fileData = request.getFileData();

	if (fileData.empty())
	{
		log(logERROR) << "Bad request: no files to create";
		throw ResponseException("400");
	}

	this->createFiles(fileData, file);
}

/**
 * @brief Handles the HTTP GET method by serving the requested file or generating a directory listing.
 *
 * This function processes an HTTP GET request by first checking if autoindexing is enabled
 * for the current location. If autoindexing is enabled, it generates a directory listing
 * and returns immediately. If autoindexing is not enabled, it extracts the file path from
 * the request URI and reads the content of the file into the response body. The content type
 * of the file is also determined and set in the response. If the file path is empty, or if
 * the file content or content type cannot be determined, it throws a ResponseException with
 * the appropriate status code.
 *
 * @param request The HTTP request to be processed.
 * @throws ResponseException if the file path is empty, or if the file content or content type cannot be determined.
 */
void	Response::getMethod(Request &request)
{
	(void)request;
	if (_loc->getAutoindex() == 1)
	{
		this->createDirlisting(_loc->getRoot());
		return ;
	}

	std::string filePath = this->extractFilePath(true);
	if (filePath.empty())
		throw ResponseException("500");

	this->_body = readFileToString(filePath);
	this->_contentType = findType(filePath);

	if (this->_body == "" || this->_contentType == "")
		throw ResponseException("404");
}

/**
 * @brief Handles the HTTP DELETE method by deleting the specified file.
 *
 * This function processes an HTTP DELETE request by extracting the file path
 * from the request URI. It then attempts to delete the file using the remove
 * function. If the file deletion fails, it logs an error message and throws
 * a ResponseException with a "500" status code.
 *
 * @param request The HTTP request containing the file path to be deleted.
 * @throws ResponseException if the file deletion fails.
 */
void	Response::deleteMethod(Request &request)
{
	(void)request;

	std::string file = this->extractFilePath(false);

	if (remove(file.c_str()) != 0)
	{
		log(logERROR) << "Failed to delete file: " << file;
		throw ResponseException("500");
	}
}

/**
 * @brief Generates the HTTP response string.
 *
 * Constructs the HTTP response based on the current status and other member variables.
 *
 * @return The complete HTTP response as a string.
 */
std::string	Response::getResponse()
{
	std::ostringstream response;
	std::pair<std::string, std::string> error;

	if (this->_status >= 400) // add error page body
	{
		try
		{
			error = _loc->findError(_status);
			this->_reason = error.first;
			this->_body = readFileToString(error.second);
			_contentType = findType(error.second);
		} catch (std::exception &e) {
			log(logERROR) << "No error page found";
		}
		_connection = "close";
	}

	response << HTTPVERSION << " " << _status << " " << _reason << "\r\n";
	response << "Date: " << getDateTime() << "\r\n";
	response << "Content-Length: " << _body.size() << "\r\n";
	if (!_contentType.empty())
		response << "Content-Type: " << _contentType << "\r\n";
	response << "Connection: " << _connection << "\r\n";
	if (!_sessionId.empty())
		response << "Set-Cookie: session_id=" << _sessionId << "\r\n";
	if (!_redir.empty())
		response << "Location: " << _redir << "\r\n";
	response << "\r\n";

	std::string return_value = response.str();
	if (_body.size())
		return_value += _body + "\r\n\r\n";

	return (return_value);
}

void	Response::setReason(std::string reason)
{
	this->_reason = reason;
}
