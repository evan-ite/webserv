#include "../includes/settings.hpp"

Request::Request(std::string httpRequest)
{
	if (httpRequest == TOOLARGE)
		this->_contentLength = -1;
	else
		this->parse(httpRequest);
}

Request::Request() {}

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
		if (this->_contentLength != -1)
			this->parseMultipart(httpRequest);
	}
	else if (method == "DELETE")
	{
		this->_method = DELETE;
		this->_location = findKey(httpRequest, "DELETE ", ' ');
	}
	else
		log(logERROR) << "Invalid http method";
	this->_userAgent = findKey(httpRequest, "User-Agent:", '\n');
	this->_host = findKey(httpRequest, "Host:", '\n');
	this->_connection = findKey(httpRequest, "Connection:", '\n');
	if (this->_contentLength != -1)
		this->_contentLength = atoi(findKey(httpRequest, "Content-Length:", '\n').c_str());
	this->_contentType = findKey(httpRequest,"Content-Type: ", '\r');
	if (this->_contentType.empty())
		this->_contentType = "application/octet-stream";
/*	log(logDEBUG)	<< "Request object created:\n" \
					<< "method " << this->_method << "\n" \
					<< "location " << this->_location << "\n" \
					<< "useragent " << this->_userAgent << "\n" \
					<< "host " << this->_host << "\n" \
					<< "connection " << this->_connection << "\n" \
					<< "contnetlen " << this->_contentLength << "\n" \
					<< "body " << this->_body;
	printFileData();
	*/
}


std::string Request::findBoundary(const std::string& httpRequest) {
	size_t pos = httpRequest.find("boundary=");
	if (pos == std::string::npos) return "";
	pos += 9; // Length of "boundary="
	size_t endPos = httpRequest.find("\r", pos);
	return "--" + httpRequest.substr(pos, endPos - pos);
}

void Request::parsePart(const std::string& part) {
	std::string::size_type headerEndPos = part.find("\r\n\r\n");
	//log(logDEBUG) << "HEADER END POS: " << headerEndPos;
	if (headerEndPos == std::string::npos) return;

	std::string headers = part.substr(0, headerEndPos);
	std::string content = part.substr(headerEndPos + 4); // Skip the CRLF

	size_t filenamePos = headers.find("filename=\"");
	if (filenamePos != std::string::npos) {
		filenamePos += 10; // Length of "filename=\""
		size_t filenameEndPos = headers.find("\"", filenamePos);
		std::string filename = headers.substr(filenamePos, filenameEndPos - filenamePos);
		_fileData.push_back(std::make_pair(filename, content));
	}
}

void Request::parseMultipart(const std::string& httpRequest) {
	std::string boundary = findBoundary(httpRequest);
	//log(logDEBUG) << "BOUNDARY: " << boundary;
	if (boundary.empty()) {
		_body = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
		return;
	}

	std::istringstream stream(httpRequest);
	std::string line;
	bool inPart = false;
	std::string part;

	while (std::getline(stream, line)) {
		if (line.find(boundary) != std::string::npos) {
			if (inPart) {
				parsePart(part);
				part.clear();
			}
			inPart = !inPart;
		} else if (inPart) {
			part += line + "\n";
		}
	}
}

void Request::printFileData() {
	if (this->_fileData.empty())
		return ;

	for (size_t i = 0; i < _fileData.size(); ++i) {
		log(logDEBUG) << "Pair " << i+1 << ": (" << _fileData[i].first << ", " << _fileData[i].second << ")\n";
	}
}

std::string		Request::getLoc() {
	return this->_location;
}

std::string		Request::getContentType() {
	return this->_contentType;
}

HttpMethod		Request::getMethod() {
	return this->_method;
}

int				Request::getContentLen() {
	return this->_contentLength;
}

std::string		Request::getBody() {
	return this->_body;
}

std::vector<std::pair<std::string, std::string> >	Request::getFileData() {
	return this->_fileData;
}

std::string		Request::getConnection() {
	return this->_connection;
}
