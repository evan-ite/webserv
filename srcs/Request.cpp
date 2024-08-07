#include "../includes/settings.hpp"

Request::Request(std::string httpRequest)
{
	this->parse(httpRequest);
}

Request::Request()
{
	this->_method = INVALID;
	this->_contentLength = -1;
	this->_status = -1;
}

Request::Request(const Request &copy)
{
	this->_method = copy._method;
	this->_host = copy._host;
	this->_contentLength = copy._contentLength;
	this->_location = copy._location;
	this->_userAgent = copy._userAgent;
	this->_connection = copy._connection;
	this->_fileData = copy._fileData;
	this->_body = copy._body;
}

Request::~Request() {}

Request & Request::operator=(const Request &assign)
{
	if (this == &assign)
		return (*this);
	this->_method = assign._method;
	this->_host = assign._host;
	this->_contentLength = assign._contentLength;
	this->_location = assign._location;
	this->_userAgent = assign._userAgent;
	this->_connection = assign._connection;
	this->_fileData = assign._fileData;
	this->_body = assign._body;
	return (*this);
}


/**
 * STATIC HELPERFUNCTION
 * @brief Extracts the body from an HTTP request.
 * This function takes an HTTP request as input and extracts the body from it.
 * The body is separated from the headers by a double newline ("\r\n\r\n").
 * If no body is found, an empty string is returned.
 *
 * @param httpRequest The HTTP request from which to extract the body.
 * @return The extracted body as a string.
 */
static std::string extractBodyFromHttpRequest(const std::string& httpRequest)
{
	size_t bodyStart = httpRequest.find("\r\n\r\n");
	if (bodyStart == std::string::npos)
		return ("");

	std::string body = httpRequest.substr(bodyStart + 4); // Skip the CRLF
	return (body);
}

/**
 * @brief Parses the HTTP request and extracts relevant information.
 *
 * This function determines the HTTP method (GET, POST, DELETE, etc.), extracts the location,
 * and processes headers such as User-Agent, Host, Connection, Transfer-Encoding, Content-Length,
 * Content-Type, and Cookie. It also handles multipart data and chunked transfer encoding.
 *
 * @param httpRequest The HTTP request as a string.
 */
void Request::parse(std::string httpRequest)
{
	std::string	method = splitReturnFirst(httpRequest, " ");
	this->_transferEncoding = findKey(httpRequest, "Transfer-Encoding: ", '\r');
	if (method == "GET")
	{
		this->_method = GET;
		this->_location = findKey(httpRequest, "GET ", ' ');
	}

	else if (method == "POST")
	{
		this->_method = POST;
		this->_location = findKey(httpRequest, "POST ", ' ');
		if (httpRequest.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			this->_contentLength = -1;
			this->_body = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
			_fileData.push_back(std::make_pair(makeName(), this->_body));
		}
		else if (this->_contentLength != -1)
			this->parseMultipart(httpRequest);
	}

	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = findKey(httpRequest, "DELETE ", ' ');
	}

	else
	{
		this->_method = INVALID;
		this->_location = "/";
		this->_status = 400;
		log(logERROR) << "Invalid http method";
	}

	this->_userAgent = findKey(httpRequest, "User-Agent:", '\r');
	this->_host = findKey(httpRequest, "Host:", '\r');
	this->_connection = findKey(httpRequest, "Connection: ", '\r');
	if (this->_connection != "close")
		this->_connection = "keep-alive";
	this->_contentLength = atoi(findKey(httpRequest, "Content-Length:", '\r').c_str());
	this->_contentType = findKey(httpRequest,"Content-Type: ", '\r');
	this->_sessionId = findKey(httpRequest,"Cookie: ", '\r');
	if (!(this->_sessionId.empty()))
	{
		ssize_t pos = this->_sessionId.find('=');
		this->_sessionId = this->_sessionId.substr(1 + pos);
	}

	if (this->_contentType.empty())
		this->_contentType = "application/octet-stream";
	this->_body = extractBodyFromHttpRequest(httpRequest);
	if (status == -1 && !this->_body.empty())
		this->_status = 0; // 0 signals to the appendStr function that some reading into the body has been done, maybe more is coming.
}

/**
 * @brief Finds the boundary string in a multipart HTTP request.
 *
 * This function searches for the boundary string used to separate parts in a multipart
 * HTTP request.
 *
 * @param httpRequest The HTTP request as a string.
 * @return The boundary string prefixed with "--", or an empty string if not found.
 */
std::string Request::findBoundary(const std::string& httpRequest)
{
	size_t pos = httpRequest.find("boundary=");
	if (pos == std::string::npos) return "";
	pos += 9; // Length of "boundary="
	size_t endPos = httpRequest.find("\r", pos);
	return ("--" + httpRequest.substr(pos, endPos - pos));
}

/**
 * @brief Parses a single part of a multipart HTTP request.
 *
 * This function extracts headers and content from a single part of a multipart HTTP request.
 * If a filename is found in the headers, it stores the filename and content as a pair.
 *
 * @param part A single part of the multipart HTTP request as a string.
 */
void Request::parsePart(const std::string& part)
{
	std::string::size_type headerEndPos = part.find("\r\n\r\n");
	if (headerEndPos == std::string::npos) return ;

	std::string headers = part.substr(0, headerEndPos);
	std::string content = part.substr(headerEndPos + 4); // Skip the CRLF

	size_t filenamePos = headers.find("filename=\"");
	if (filenamePos != std::string::npos)
	{
		filenamePos += 10; // Length of "filename=\""
		size_t filenameEndPos = headers.find("\"", filenamePos);
		std::string filename = headers.substr(filenamePos, filenameEndPos - filenamePos);
		_fileData.push_back(std::make_pair(filename, content));
	}
}

/**
 * @brief Parses a multipart HTTP request.
 *
 * This function processes a multipart HTTP request by finding the boundary string and
 * extracting each part. It calls parsePart() for each part found.
 *
 * @param httpRequest The HTTP request as a string.
 */
void Request::parseMultipart(const std::string& httpRequest)
{
	std::string boundary = findBoundary(httpRequest);
	if (boundary.empty())
	{
		_body = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
		return ;
	}
	std::istringstream stream(httpRequest);
	std::string line;
	bool inPart = false;
	std::string part;

	while (std::getline(stream, line))
	{
		if (line.find(boundary) != std::string::npos)
		{
			if (inPart)
			{
				parsePart(part);
				part.clear();
			}
			inPart = !inPart;
		}
		else if (inPart)
			part += line + "\n";
	}
}

/**
 * @brief Generates a unique name based on the current timestamp.
 *
 * This function creates a unique name string using the current local time formatted as
 * "YYYY-MM-DD-HH-MM-SS".
 *
 * @return The generated name as a string.
 */
std::string Request::makeName()
{
	//generate a name based on the time stamp
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
	std::string str(buffer);
	return (str);
}

/**
 * @brief Prints the file data stored in the request.
 *
 * This function logs each pair of file data stored in the request. If there is no file data,
 * the function returns immediately.
 */
void Request::printFileData()
{
	if (this->_fileData.empty())
		return ;

	for (size_t i = 0; i < _fileData.size(); ++i)
	{
		log(logDEBUG) << "Pair " << i+1 << ": (" << _fileData[i].first << ", " << _fileData[i].second << ")\n";
	}
}

/**
 * Appends a string to the raw request string and checks if the request is complete.
 * @param str The string to append to the raw request string.
 * @param count The number of characters to append from the string.
 * @return True if the request is complete and valid, false otherwise.
 */
bool Request::appendString(std::string &str, size_t count)
{
	if(this->_status != -1)
		this->_body.append(str, count);
	else
	{
		this->_rawString.append(str, count);
		if (this->_rawString.find("\r\n\r\n") != std::string::npos)
		{
			this->parse(this->_rawString);
			return (this->validate());
		}
	}
	return (true);
}

bool Request::validate()
{
	if (this->_method == INVALID)
	{
		log(logERROR) << "Invalid http method";
		return (false);
	}
	if (this->_location.empty())
	{
		log(logERROR) << "Invalid location";
		return (false);
	}
	if (this->_contentLength < 0)
	{
		log(logERROR) << "Invalid content length";
		return (false);
	}
	return (true);
}

std::string		Request::getLoc()
{
	return (this->_location);
}

void 	Request::setLoc(std::string &location)
{
	this->_location = location;
}


std::string		Request::getContentType()
{
	return (this->_contentType);
}

HttpMethod		Request::getMethod()
{
	return (this->_method);
}

int				Request::getContentLen()
{
	return (this->_contentLength);
}

std::string		Request::getBody()
{
	return (this->_body);
}

std::vector<std::pair<std::string, std::string> >	Request::getFileData()
{
	return (this->_fileData);
}

std::string		Request::getConnection()
{
	return (this->_connection);
}

void			Request::setConnection(std::string connection)
{
	this->_connection = connection;
}


std::string		Request::getsessionId()
{
	return (this->_sessionId);
}

void			Request::resetSessionId()
{
	this->_sessionId = "";
}
