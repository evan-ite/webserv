#include "../includes/settings.hpp"

Request::Request(): _foundHeader(false) {}

Request::Request(Location &location): _foundHeader(false), _location(location) {}

Request::~Request() {}

/**
 * @brief Appends buffer data to the request and processes the header if found.
 *
 * Appends data from the buffer to the raw request string. If the header hasn't been found yet,
 * it searches for the end of the header ("\r\n\r\n"), parses it, and appends the body.
 * If the header is already found, it appends data directly to the body.
 *
 * @param buffer The buffer containing the data to append.
 * @param bytes_read The number of bytes read from the buffer.
 * @return true if data was successfully appended and processed, false otherwise.
 */
bool	Request::appendBuffer(char *buffer, int bytes_read)
{
	std::string::size_type end_pos = std::string::npos;

	_rawReq.append(buffer, bytes_read);
	// log(logDEBUG) << "raw request ------------\n" << _rawReq;
	if (!_foundHeader)
	{
		if ((end_pos = _rawReq.find("\r\n\r\n")) == std::string::npos)
			return (false);
		parseHeader(_rawReq.substr(0, end_pos));
		_foundHeader = true;
		_body.append(_rawReq, end_pos + 4, _rawReq.size() - (end_pos + 4));
	}
	else
		_body.append(buffer, bytes_read);

	if (isBodyComplete())
	{
		parseBody();
		return (true);
	}
	return (false);
}

/**
 * @brief Parses the HTTP header and extracts relevant information.
 *
 * This function assigns the HTTP method and URI, and extracts various header fields
 * such as Content-Type, Content-Length, Connection, Cookie, Transfer-Encoding, and boundary.
 * It also handles special cases for chunked transfer encoding and default content type.
 *
 * @param header The HTTP header as a string.
 */
void Request::parseHeader(std::string header)
{
	assignMethodAndUri(header);
	_contentType = findKey(header, "Content-Type: ", ';');
	_contentLength = atoi(findKey(header, "Content-Length: ", '\r').c_str());
	_connection = findKey(header, "Connection: ", '\r');
	_sessionId = findKey(header, "Cookie: ", '\r');
	_transferEncoding = findKey(header, "Transfer-Encoding: ", '\r');
	_boundary = findKey(header, "boundary=", '\r');
	_serverName = findKey(header, "Host: ", '\r');
	size_t colon = _serverName.find(":");
	if (colon != std::string::npos)
		_serverName = _serverName.substr(0, colon);
	if (_transferEncoding == "chunked")
		_contentLength = -1;
	if (_contentType.empty())
		_contentType = "application/octet-stream";
	if (!(_sessionId.empty())) {
		ssize_t pos = _sessionId.find('=');
		_sessionId = _sessionId.substr(1 + pos);
	}
}

/**
 * @brief Assigns the HTTP method and URI from the header.
 *
 * This function extracts the HTTP method (GET, POST, DELETE) and URI from the header.
 * If the method is invalid, it sets the method to INVALID and the URI to "/".
 *
 * @param header The HTTP header as a string.
 */
void Request::assignMethodAndUri(std::string header)
{
	std::string	method = splitReturnFirst(header, " ");
	if (method == "GET")
	{
		_method = GET;
		_uri = findKey(header, "GET ", ' ');
	}
	else if (method == "POST")
	{
		_method = POST;
		_uri = findKey(header, "POST ", ' ');
	}
	else if (method == "DELETE")
	{
		_method = DELETE;
		_uri = findKey(header, "DELETE ", ' ');
	}
	else {
		_method = INVALID;
		_uri = "/";
		log(logERROR) << "Invalid http method";
	}
}

/**
 * @brief Parses the HTTP body based on the method and content type.
 *
 * This function handles the parsing of the HTTP body for POST requests.
 * It supports multipart/form-data and chunked transfer encoding.
 *
 * If the body is empty or the method is not POST, the function returns immediately.
 */
void Request::parseBody()
{
	if (_body.empty())
		return ;
	if (_method == POST)
	{
		if (_contentType.find("multipart/form-data") != std::string::npos) {
			parseMultipart();
		}
		else if (_transferEncoding == "chunked")
		{
			unchunkBody();
			_fileData.push_back(std::make_pair(makeName(), _body));
		}
	}
}

/**
 * @brief Parses a multipart HTTP request.
 *
 * This function processes a multipart HTTP request by finding the boundary string and
 * extracting each part. It calls parsePart() for each part found.
 */
void Request::parseMultipart()
{
	if (_boundary.empty())
		return ;
	std::istringstream stream(_body);
	std::string line;
	bool inPart = false;
	std::string part;

	while (std::getline(stream, line))
	{
		if (line.find(_boundary) != std::string::npos)
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

	std::string header = part.substr(0, headerEndPos);
	std::string content = part.substr(headerEndPos + 4); // Skip the CRLF

	size_t filenamePos = header.find("filename=\"");
	if (filenamePos != std::string::npos)
	{
		filenamePos += 10; // Length of "filename=\""
		size_t filenameEndPos = header.find("\"", filenamePos);
		std::string filename = header.substr(filenamePos, filenameEndPos - filenamePos);
		_fileData.push_back(std::make_pair(filename, content));
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
 * @brief Unchunks the HTTP body.
 *
 * This function processes the chunked transfer encoding of the HTTP body.
 * It reads each chunk size, extracts the corresponding chunk data, and
 * assembles the complete body by removing the chunked encoding.
 */
void	Request::unchunkBody()
{
	std::string::size_type pos = 0;
	std::string::size_type next_pos = 0;
	std::string chunk_size;
	std::string newBody;

	while (pos < _body.size())
	{
		next_pos = _body.find("\r\n", pos);
		if (next_pos == std::string::npos)
			break;

		chunk_size = _body.substr(pos, next_pos - pos);
		size_t size = 0;
		std::istringstream iss(chunk_size);
		iss >> std::hex >> size;
		if (iss.fail())
			break;
		if (size == 0)
		{
			pos = _body.find("\r\n", pos);
			if (pos != std::string::npos)
				pos += 2;
			continue;
		}

		pos = next_pos + 2;
		next_pos = pos + size;
		if (next_pos > _body.size())
			break;

		newBody.append(_body, pos, size);
		pos = next_pos + 2;
	}
	_body = newBody;
}

/**
 * @brief Validates the HTTP request.
 *
 * This function checks various aspects of the HTTP request to ensure it is valid.
 * It verifies the allowed methods, URI, connection header, content length, and
 * presence of content for POST requests. If any validation fails, it sets the
 * appropriate status code and throws a RequestException.
 *
 * @throws RequestException if the request is invalid.
 */
void Request::isValidRequest()
{
	if (!_location.findAllow(_method))
	{
		log(logERROR) << "Invalid request: method not allowed.";
		_status = 405;
		throw RequestException("405");
	}
	if (_uri.find("..") != std::string::npos)
	{
		log(logERROR) << "Invalid request: path contains double dots.";
		_status = 400;
		throw RequestException("400");
	}
	if (_connection != "keep-alive" && _connection != "close")
	{
		if (_connection.empty())
			_connection = "keep-alive";
		else
		{
			log(logERROR) << "Invalid request: connection header is invalid.";
			_status = 400;
			throw RequestException("400");
		}
	}
	if (_contentLength / 1024 > _location.getClientMaxBodySize())
	{
		log(logERROR) << "Invalid request: content length exceeds client_max_body_size.";
		_status = 413;
		throw RequestException("413");
	}
	if (_method == POST && _contentLength <= 0 && _transferEncoding != "chunked")
	{
		log(logERROR) << "Invalid request: no content found.";
		_status = 400;
		throw RequestException("400");
	}
	if (!_location.getServerName().empty() && (_serverName.empty() || _serverName != _location.getServerName()))
	{
		log(logERROR) << "Invalid request: Server name does not match";
		_status = 400;
		throw RequestException("400");
	}
}

void Request::checkLength()
{
	if (_foundHeader && _contentLength / 1024 > _location.getClientMaxBodySize())
	{
		log(logERROR) << "Invalid request: content length exceeds client_max_body_size.";
		_status = 413;
		throw RequestException("413");
	}
}

/**
 * @brief Sets the location for the request.
 * This function sets the location for the request to the specified location object.
 * @param location The location object to set.
 */
void Request::setLocation(Location& location)
{
	_location =	location;
}

/**
 * @brief Returns the location associated with the request.
 * @return A reference to the Location object associated with the request.
 */
Location& Request::getLocation()
{
	return (_location);
}

/**
 * @brief Prints the HTTP request details.
 *
 * This function assembles the details of the HTTP request into a formatted string.
 * It includes the method, URI, content type, content length, connection, session ID,
 * transfer encoding, body, boundary, and file data.
 *
 * @return std::ostream& The formatted request details.
 */
std::string  Request::printRequest()
{
	std::ostringstream oss;
	oss << "Method: " << _method << std::endl;
	oss << "URI: " << _uri << std::endl;
	oss << "Content-Type: " << _contentType << std::endl;
	oss << "Content-Length: " << _contentLength << std::endl;
	oss << "Connection: " << _connection << std::endl;
	oss << "Session ID: " << _sessionId << std::endl;
	oss << "Transfer-Encoding: " << _transferEncoding << std::endl;
	//oss << "Body: " << _body << std::endl;
	oss << "Boundary: " << _boundary << std::endl;
	oss << "File data: " << std::endl;

	for (std::vector<std::pair<std::string, std::string> >::iterator it = _fileData.begin(); it != _fileData.end(); ++it)
	{
		oss << "Filename: " << it->first << std::endl;
		//oss << "Content: " << it->second << std::endl;
	}
	return (oss.str());
}

std::vector<std::pair<std::string, std::string> >	Request::getFileData()
{
	return (_fileData);
}

bool Request::isBodyComplete()
{
	if (_contentType == "multipart/form-data" ) {
		size_t firstBoundaryPos = _body.find_first_of(_boundary);
		size_t lastBoundaryPos = _body.rfind(_boundary);

		if (firstBoundaryPos == std::string::npos || lastBoundaryPos == std::string::npos)
			return false;
		if (firstBoundaryPos == lastBoundaryPos)
			return false;
		if (_body.substr(lastBoundaryPos + _boundary.length(), 2) != "--")
			return false;
		return (true);
	}
	else if (_transferEncoding == "chunked" && _body.find("\r\n0\r\n") != std::string::npos)
		return (true);
	else if (_transferEncoding != "chunked" && _contentType != "multipart/form-data" && _contentLength == static_cast<int>(_body.size()))
		return (true);
	else
		return (false);
}
