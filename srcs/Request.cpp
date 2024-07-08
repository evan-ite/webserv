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
	return *this;
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
		parseMultipart(httpRequest);
	}
	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = findKey(httpRequest, "DELETE ", ' ');
	}
	else
		log(logERROR) << "Invlaid http mehtod\n" << httpRequest;
		// throw Request::invalidMethod();
	this->_userAgent = findKey(httpRequest, "User-Agent:", '\n');
	this->_host = findKey(httpRequest, "Host:", '\n');
	this->_connection = findKey(httpRequest, "Connection:", '\n');
	this->_contentLenght = atoi(findKey(httpRequest, "Content-Length:", '\n').c_str());
	log(logDEBUG) << "Request object created:\n" << this->_method << "\n" << this->_location << "\n" << this->_userAgent << "\n" << this->_host << "\n" << this->_connection << "\n" << this->_contentLenght;
	printFileData();
}

void Request::parseMultipart(std::string httpRequest)
{
	std::string body = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4); // Find body
	std::string boundary = "--" + (findKey(httpRequest, "boundary=", '\r').substr(9)); // Find bounndary
	std::string::size_type startPos = body.find(boundary) + boundary.length() + 2; // Skip the boundary and CRLF
	while (startPos != std::string::npos)
	{
		std::string::size_type endPos = body.find(boundary, startPos) - 4; // Find the end boundary and subtract the CRLF
		std::string part = body.substr(startPos, endPos - startPos); // This is the actual info we need
		// Parse headers within the part
		if (part == "\r\n")
			break;
		std::string::size_type headerEndPos = part.find("\r\n\r\n");
		std::string headers = part.substr(0, headerEndPos);
		std::string content = part.substr(headerEndPos + 4); // Skip the CRLF
		// Parse Content-Disposition header
		std::string disposition = findKey(headers, "Content-Disposition: ", '\r');
		if (disposition.find("filename=\"") != std::string::npos) {
			std::string filename = disposition.substr(disposition.find("filename=\"") + 10);
			filename = filename.substr(0, filename.find("\""));
			// Store the file content
			this->_fileData[filename] = content;
		}
		startPos = body.find(boundary, endPos + 4) + boundary.length() + 2;
	}
}

void Request::printFileData() {
	if (this->_fileData.empty())
		return ;
	// Define a type for the map iterator
	typedef std::map<std::string, std::string>::const_iterator MapIterator;

	// Iterate over the map using iterators
	for (MapIterator iter = _fileData.begin(); iter != _fileData.end(); ++iter) {
		log(logDEBUG) << "Filename: " << iter->first << "\nContent: " << iter->second << "\n";
	}
}
