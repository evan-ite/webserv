#include "../includes/server.hpp"

// Constructors
Response::Response() {}

Response::Response(std::string const &httpRequest, Server serverData)
{
	Request request(httpRequest);
	Location loc = serverData.locations["/"];
	std::string index = loc.index;
	std::string root = loc.root;

	int cgi = (request._location.size() > 4 && 
           request._location.substr(request._location.size() - 4) == ".cgi") ||
          (request._location.find(".cgi?") != std::string::npos);
		//    && serverData.cgi == "on"

	try {
		if (cgi) 
			cgiMethod(request, serverData);
		else if (request._method == POST)
			postMethod(request, serverData);
		else if (request._method == GET)
			getMethod(request, serverData, root, index);
		log(logDEBUG) << "Response object succesfully created";
	}
	catch (ResponseException &e) {
        // Handle other methods or send a 405 Method Not Allowed response
        this->_status = 405;
        this->_reason = "Method Not Allowed";
        this->_type = "text/plain";
        this->_body = readFileToString("content/error/405.html");
        this->_connection = "close";
        this->_len = _body.length();
        this->_date = getDateTime();
    }
}

/* Sets date and time to moment of copy */
Response::Response(const Response &copy) :
	_status(copy._status),
	_reason(copy._reason),
	_type(copy._type),
	_len(copy._len),
	_date(getDateTime()),
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
	this->_date = assign._date;
	return (*this);
}

std::string Response::makeResponse()
{
	std::ostringstream response;

	response << HTTPVERSION << " " << this->_status << " " << this->_reason << "\r\n";
	response << "Date: " << this->_date << "\r\n";
	response << "Content-Length: " << this->_len << "\r\n";
	if (this->_type != "")
		response << "Content-Type: " << this->_type << "\r\n";
	response << "Connection: " << this->_connection << "\r\n";
	response << "\r\n";
	std::string return_value = response.str();
	if (this->_len)
		return_value += this->_body + "\r\n";
	return (return_value);
}

void	Response::postMethod(Request request, Server serverData)
{

	(void) serverData;
	
	// Create all files
	for (size_t i = 0; i < request._fileData.size(); ++i) {

		std::string filename = request._fileData[i].first;
		std::string content = request._fileData[i].second;

		std::cout << "Filename: " << filename << "\nContent: " << content << "\n";
	}

	// Process the POST data (e.g., save it, respond with a success message, etc.)
	this->_status = 200;
	this->_body = "Received POST data: ";
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = "text/plain";
	this->_connection = "close"; // Generally, you close the connection after handling POST
	this->_date = getDateTime();
}

void	Response::getMethod(Request request, Server serverData, std::string root, std::string index)
{
	(void) serverData;

	std::string file = root + request._location;
	log(logDEBUG) << "get file : " << file;
	if (request._location == "/")
		file = root + "/" + index;
	this->_status = 200;
	this->_body = readFileToString(file);
	this->_len = _body.length();
	this->_reason = "ok";
	this->_type = findType(file);
	this->_connection = "keep-alive";
	this->_date = getDateTime();

	// Check if body is empty or type was not found
	if (this->_body == "" || this->_type == "") {
		this->_status = 404;
		this->_reason = "not found";
		this->_type = "";
		this->_body = readFileToString("content/error/404.html");;
		this->_connection = "close";
		this->_len = 0;
	}
}


void	Response::deleteMethod() {}

void Response::cgiMethod(Request request, Server serverData)
{
	(void) serverData;

	log(logINFO) << "Using CGI to fetch data";

    // Determine the path to the CGI script based on the request
    std::string cgiScriptPath = "content/cgi-bin/simple.py"; //serverData.cgi_bin + request._location;

    // Set up environment variables specific to GET or POST
    if (request._method == GET) {
    	setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", findKey(request._location, "?", ' ').c_str(), 1);
    } else if (request._method == POST) {
    	setenv("REQUEST_METHOD", "POST", 1);
        setenv("CONTENT_TYPE", request._contentType.c_str(), 1);
        setenv("CONTENT_LENGTH", to_string(request._contentLenght).c_str(), 1); 
	}
	// Set up common environment variables required by the CGI script
    setenv("SCRIPT_NAME", cgiScriptPath.c_str(), 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("SERVER_SOFTWARE", "MyServer/1.0", 1);


    // Prepare to capture the CGI script's output
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        log(logERROR) << "Error creating pipe";
		throw ResponseException();
	}

    pid_t pid = fork();
    if (pid == -1) {
        log(logERROR) << "Error forking";
		throw ResponseException();
	}
	else if (pid == 0) {
        // Child process: execute the CGI script
        close(pipefd[0]);  // Close read end of the pipe
		// If POST, write the data to the pipe
        if (request._method == POST) {
            write(pipefd[1], this->_body.c_str(), this->_len);
        }

        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        dup2(pipefd[1], STDERR_FILENO);  // Redirect stderr to the pipe
        close(pipefd[1]);  // Close write end of the pipe
		
        // Prepare arguments for execve
        char *args[] = { strdup(cgiScriptPath.c_str()), NULL };

        // Convert environment variables to char* array
        extern char **environ;
        execve(cgiScriptPath.c_str(), args, environ);

        log(logERROR) << "Error executing cgi script";
		throw ResponseException();
    } else {
        log(logDEBUG) << "Back in parent process";
        // Parent process: read the CGI script's output
        close(pipefd[1]);  // Close write end of the pipe

        std::string cgiOutput;
        char buffer[512];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            cgiOutput.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        // Process the CGI output and create the HTTP response
        if (!cgiOutput.empty()) {
            this->_status = 200;
            this->_body = cgiOutput;
            this->_len = _body.length();
            this->_reason = "OK";
            this->_type = "text/html";  // Adjust based on CGI output
            this->_connection = "close";  // Close the connection after handling request
            this->_date = getDateTime();
        } else {
            // Handle case where CGI script produces no output
        	log(logERROR) << "CGI output empty";
            this->_status = 500;
            this->_body = readFileToString("content/error/500.html");
            this->_len = _body.length();
            this->_reason = "Internal Server Error";
            this->_type = "text/html";
            this->_connection = "close";
            this->_date = getDateTime();
        }
    }
}
