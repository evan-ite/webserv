#include "../includes/settings.hpp"

// Constructors
Response::Response() {}

Response::Response(int	status,
		std::string	reason,
		std::string	type,
		std::string	connection,
		std::string	body)
{
	this->_status = status;
	this->_reason = reason;
	this->_type = type;
	this->_len = body.size();
	this->_connection = connection;
	this->_body = body;
}

Response::Response(Request &request, Location &loc)
{
	try
	{
		this->_loc = loc;
		HttpMethod method = request.getMethod();
		if (request.getsessionId().empty())
			this->_sessionId = generateRandomString(12);
		else
			this->_sessionId = "";

		if (!isValidRequest(request))
			throw ResponseException("400");
		else if (this->handleRedir(loc.getRedir()))
			return ;
		else if (this->handleCGI(request))
			return ;
		this->checkMethod(method, request);
		log(logDEBUG) << "Response object succesfully created";
	}
	catch (std::exception &e)
	{
		std::pair<std::string, std::string> error = loc.findError(e.what());
		this->_status = atoi(e.what());
		this->_reason = error.first;
		this->_type = "text/html";
		this->_body = readFileToString(error.second);
		this->_connection = "close";
		this->_len = _body.length();
	}
}

void	Response::postMethod(Request &request)
{
	int status = 0;
	createFiles(request, status);
	switch (status)
	{
		case 201:
			this->_status = 201;
			this->_reason = getStatusMessage("201");
			this->_type = "text/html";
			this->_connection = request.getConnection();
			this->_body = "";
			this->_len = _body.length();
			break;
		case 500:
			throw ResponseException("500");
		case 400:
			throw ResponseException("400");
	}
}

void	Response::getMethod(Request &request)
{
	std::string filePath = this->extractFilePath(request);

	this->_status = 200;
	if (!filePath.empty())
	{
		this->_body = readFileToString(filePath);
		this->_type = findType(filePath);
	}
	else
		this->_type = "text/html";
	this->_len = _body.length();
	this->_reason = getStatusMessage("200");

	this->_connection = request.getConnection();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "")
		throw ResponseException("404");
}

void	Response::deleteMethod(Request &request)
{
	std::string file = this->_loc.getRoot() + request.getLoc();

	if (remove(file.c_str()) != 0)
	{
		log(logERROR) << "Failed to delete file: " << file;
		throw ResponseException("404");
	}
	else {
		this->_status = 200;
		this->_reason = getStatusMessage("200");
		this->_type = "text/html";
		this->_connection = request.getConnection();
		this->_body = "";
		this->_len = _body.length();
	}
}

// Creates an html page with name fileName that lists the content of dirPath
void	Response::createDirlisting(std::string dirPath)
{
	std::string htmlTemplate = this->_dirlistTemplate;

	std::size_t pos = htmlTemplate.find("INSERT");
	if (pos == std::string::npos)
		{log(logERROR) << "Invalid directory listing template";}
	std::string insertList = this->loopDir(dirPath);
	htmlTemplate.replace(pos, 6, insertList);

	while((pos = htmlTemplate.find("PATH")) != std::string::npos)
		htmlTemplate.replace(pos, 4, this->_loc.getPath());

	this->setBody(htmlTemplate);

}

// Function that loops through directory and subdirectories and
// creates html list of the content
std::string	Response::loopDir(std::string dirPath)
{
	std::ostringstream html;
	if (dirPath[0] == '/' || dirPath[0] == '.') // Check if path starts with / or .
		dirPath = dirPath.substr(1);
	dirPath = this->_loc.getRoot() + "/" + dirPath;

	struct dirent	*entry;
	DIR		*dir = opendir(dirPath.c_str());
	if (dir == NULL)
	{
		log(logERROR) << "Error opening directory: " << dirPath;
		throw ResponseException("500");
	}

	// Loop through directory and create a list in html
	html << "<ul>";
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			html << "<li> "
			<< entry->d_name
			<< "<button onclick=\"deleteFile('"
			<< entry->d_name
			<< "')\">Delete</button>"
			<< "</li>";
			if (entry->d_type == DT_DIR)
			{
				std::string	subPath = dirPath + "/" + entry->d_name;
				html << this->loopDir(subPath);
			}
		}
	}
	html << "</ul>";

	if (closedir(dir) != 0)
	{
		log(logERROR) << "Error closing directory: " << dirPath;
		throw ResponseException("500");
	}
	return (html.str());
}

std::string Response::makeResponse()
{
	std::ostringstream response;

	response << HTTPVERSION << " " << this->_status << " " << this->_reason << "\r\n";
	response << "Date: " << getDateTime() << "\r\n";
	response << "Content-Length: " << this->_len << "\r\n";
	if (this->_type != "")
		response << "Content-Type: " << this->_type << "\r\n";
	response << "Connection: " << this->_connection << "\r\n";
	if (!this->_sessionId.empty())
		response << "Set-Cookie: session_id=" << this->_sessionId << "\r\n";
	if (!this->_redir.empty())
		response << "Location: " << this->_redir << "\r\n";
	response << "\r\n";
	std::string return_value = response.str();
	if (this->_len)
		return_value += this->_body + "\r\n\r\n";
	return (return_value);
}

Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_connection(copy._connection),
	_body(copy._body)
{}

// Destructor
Response::~Response() {}

// Operators
Response & Response::operator=(const Response &assign)
{
	this->_status = assign._status;
	this->_reason = assign._reason;
	this->_type = assign._type;
	this->_connection = assign._connection;
	this->_body = assign._body;
	this->_len = assign._len;
	this->_loc = assign._loc;
	return (*this);
}

void	Response::setStatus(int status)
{
	this->_status = status;
}

void	Response::setReason(std::string reason)
{
	this->_reason = reason;
}

void	Response::setType(std::string type)
{
	this->_type = type;
}

void	Response::setBody(std::string body)
{
	this->_body = body;
	this->_len = body.size();
}

void	Response::setConnection(std::string connection)
{
	this->_connection = connection;
}

std::string	Response::getConnection()
{
	return (this->_connection);
}

std::string	Response::getSessionId()
{
	return (this->_sessionId);
}
