#include "../includes/server.hpp"

Request::Request(std::string httpRequest)
{
	this->parse(httpRequest);
}

Request::Request() {}

Request::Request(const Request &copy)
{
	this->_method = copy._method;
	this->_host = copy._host;
	this->_contentLenght = copy._contentLenght;
	this->_location = copy._location;
	this->_userAgent = copy._userAgent;
	this->_connection = copy._connection;
}

Request::~Request() {}

Request & Request::operator=(const Request &assign)
{
	this->_method = assign._method;
	this->_host = assign._host;
	this->_contentLenght = assign._contentLenght;
	this->_location = assign._location;
	this->_userAgent = assign._userAgent;
	this->_connection = assign._connection;
	return (*this);
}

void Request::parse(std::string httpRequest)
{
	std::string	method = splitReturnFirst(httpRequest, " ");
	if (method == "GET")
	{
		this->_method = GET;
		this->_location = findKey(httpRequest, "GET ", ' ');
	}
	else if (method == "POST")
	{
		this->_method = POST;
		this->_location = findKey(httpRequest, "POST ", ' ');
		this->parseMultipart(httpRequest);
	}
	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = findKey(httpRequest, "DELETE ", ' ');
	}
	else
		log(logERROR) << "Invalid http method" << httpRequest;
	this->_userAgent = findKey(httpRequest, "User-Agent:", '\n');
	this->_host = findKey(httpRequest, "Host:", '\n');
	this->_connection = findKey(httpRequest, "Connection:", '\n');
	this->_contentLenght = atoi(findKey(httpRequest, "Content-Length:", '\n').c_str());
	this->_contentType = findKey(httpRequest,"Content-Type: ", '\r');
	if (this->_contentType.empty())
    	this->_contentType = "application/octet-stream"; 
	log(logDEBUG) << "Request object created:\n" << this->_method << "\n" << this->_location << "\n" << this->_userAgent << "\n" << this->_host << "\n" << this->_connection << "\n" << this->_contentLenght << "\n body: " << this->_body;
	printFileData();
}

void Request::parseMultipart(std::string httpRequest)
{
	int			i = 0;
	std::string body = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4); // Find body
	// log(logDEBUG) << "MULTIPART BODY\n" << body;
	std::string boundary = findKey(httpRequest, "boundary=", '\r'); // Find boundary
	if (boundary.empty()) {
		this->_body = body;
		return;
	}
	std::string::size_type startPos = body.find("--" + boundary) + boundary.length() + 4; // Skip the boundary and CRLF
	// Loop over all files in body
	while (startPos != std::string::npos)
	{
		// Find the end / in-between boundary and subtract the CRLF
		std::string::size_type endPos = body.find("--" + boundary, startPos) - 2;
		if (endPos == std::string::npos)
			endPos = body.find("--" + boundary + "--", startPos) - 2;
		// Substract part in between boundaries
		std::string part = body.substr(startPos, endPos - startPos);
		if (part == "\r\n")
			break;
		// Parse headers and content
		std::string::size_type headerEndPos = part.find("\r\n\r\n");
		std::string headers = part.substr(0, headerEndPos);
		std::string content = part.substr(headerEndPos + 4); // Skip the CRLF
		// Parse Content-Disposition header
		std::string disposition = findKey(headers, "Content-Disposition: ", '\r');
		if (disposition.find("filename=\"") != std::string::npos) {
			std::string filename = disposition.substr(disposition.find("filename=\"") + 10);
			filename = filename.substr(0, filename.find("\""));
			// Store the file content
			this->_fileData.push_back(std::make_pair(filename, content ));
		}
		startPos = body.find(boundary, endPos + 4) + boundary.length() + 2;
		i++;
	}
}

void Request::printFileData() {
	if (this->_fileData.empty())
		return ;

	for (size_t i = 0; i < _fileData.size(); ++i) {
        log(logDEBUG) << "Pair " << i+1 << ": (" << _fileData[i].first << ", " << _fileData[i].second << ")\n";
    }
}
