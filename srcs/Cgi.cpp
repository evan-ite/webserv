#include "../includes/settings.hpp"

// Constructors
Cgi::Cgi(): _isTrue(false) {}

/**
 * Cgi Constructor
 *
 * Initializes a Cgi object based on the provided request, server settings, and location.
 *
 * @param request Pointer to a Request object containing the HTTP request details.
 * @param serverData Pointer to a ServerSettings object containing the server configuration.
 * @param loc Pointer to a Location object containing the location-specific configuration.
 *
 * The constructor determines the CGI extension to use based on the location and server settings.
 * It sets the _isTrue flag to true if the request's location matches the CGI extension, otherwise false.
 * If neither the location nor the server settings have CGI enabled, the constructor sets _isTrue to false and returns early.
 *
 * Member variables initialized:
 * - _env: Set to NULL.
 * - _isTrue: Set based on whether the request's location matches the CGI extension.
 * - _request: Set to the provided request if _isTrue is true.
 * - _serverData: Set to the provided serverData if _isTrue is true.
 * - _loc: Set to the provided loc if _isTrue is true.
 */
Cgi::Cgi(Request *request, ServerSettings *serverData, Location *loc)
{
	this->_env = NULL;
	this->_request = request;
	this->_serverData = serverData;
	this->_loc = loc;

	std::string ext = this->getExtension();
	if (ext.empty())
		return;

	if (this->checkUri(ext))
		this->_isTrue = true;
	else
		this->_isTrue = false;
}

/**
 * Copy Constructor
 * This constructor currently does not perform any deep copy operations and only suppresses the unused parameter warning.
 */
Cgi::Cgi(const Cgi &copy)
{
	(void) copy;
}


/**
 * Destructor
 *
 * Cleans up resources used by the Cgi object.
 * If the _env member variable is not NULL, it deletes the allocated memory to prevent memory leaks.
 */
Cgi::~Cgi()
{
	if (this->_env)
		delete this->_env;
}


/**
 * Assignment Operator
 * This operator currently does not perform any deep copy operations and only suppresses the unused parameter warning.
 */
Cgi & Cgi::operator=(const Cgi &assign)
{
	(void) assign;
	return (*this);
}

/**
 * isTrue Method
 *
 * Checks if the CGI processing is enabled for the current request.
 *
 * @return A boolean value indicating whether CGI processing is enabled (true) or not (false).
 *
 * This method returns the value of the _isTrue member variable, which is set during the construction of the Cgi object.
 * If _isTrue is true, it means the request's location matches the CGI extension and CGI processing is enabled.
 * If _isTrue is false, CGI processing is not enabled for the current request.
 */
bool	Cgi::isTrue()
{
	return (this->_isTrue);
}

/**
 * execute Method
 *
 * Executes the CGI script and processes its output to create an HTTP response.
 *
 * @param response Reference to a Response object where the CGI output will be stored.
 *
 * This method checks if CGI processing is enabled, extracts the CGI script path and interpreter,
 * creates the environment variables, sets up a pipe, forks the process, and handles the child and parent processes.
 * If any error occurs, it logs the error and throws a CgiException with a 500 status code.
 */
void	Cgi::execute(Response &response)
{
	if (!this->_isTrue)
		return ;

	log(logINFO) << "Creating html dynamically with CGI";
	std::string cgiScriptPath;
	std::string interpreter;
	this->extractCgi(cgiScriptPath, interpreter);
	if (interpreter.empty()) {
		log(logERROR) << "CGI interpreter not defined";
		throw CgiException("500");
	}
	this->_env = createEnv(cgiScriptPath);

	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		log(logERROR) << "Error creating pipe";
		throw CgiException("500");
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		log(logERROR) << "Error forking";
		throw CgiException("500");
	}
	else if (pid == 0)
		this->executeCgiChild(pipefd, cgiScriptPath, interpreter);
	else
		this->executeParent(pid, pipefd, response);
}

/**
 * executeParent Method
 *
 * Handles the parent process after forking to execute the CGI script.
 *
 * @param pid Process ID of the child process executing the CGI script.
 * @param pipefd Pointer to an array of two integers representing the pipe file descriptors.
 * @param response Reference to a Response object where the CGI output will be stored.
 *
 * This method waits for the child process to complete, reads the CGI script's output from the pipe,
 * and processes the output to create the HTTP response. If the CGI output is empty or an error occurs,
 * it throws a CgiException with a 500 status code.
 */
void	Cgi::executeParent(int pid, int *pipefd, Response &response)
{
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		throw CgiException("500");
	log(logDEBUG) << "Back in parent process";
	close(pipefd[1]);
	std::string cgiOutput = this->readCgiOutput(pipefd);
	close(pipefd[0]);

	if (!cgiOutput.empty())
		this->createResponse(response, cgiOutput);
	else
		throw CgiException("500");
}

/**
 * executeCgiChild Method
 *
 * Executes the CGI script in the child process. A timeout is set with
 * alarm() to prevent infinite execution
 *
 * @param pipefd Pointer to an array of two integers representing the pipe file descriptors.
 * @param cgiPath String containing the path to the CGI script.
 * @param interpreter String containing the path to the CGI interpreter.
 *
 * This method handles the following:
 * - Writes the request body to the pipe if the request method is POST.
 * - Redirects the pipe's ends to standard input and output.
 * - Executes the CGI script using the provided interpreter and path.
 * - Logs errors and throws a CgiException if execution fails.
 */
void	Cgi::executeCgiChild(int *pipefd, std::string cgiPath, std::string interpreter)
{
	Request *request = this->_request;
	alarm(CGI_TIMEOUT);

	if (request->getMethod() == POST)
		write(pipefd[1], request->getBody().data(), request->getBody().size());

	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);

	log(logDEBUG) << "executing cgi script: '" << interpreter << " " << cgiPath << "'";
	char *args[] = { strdup(interpreter.c_str()), strdup(cgiPath.c_str()), NULL };
	execve(interpreter.c_str(), args, this->_env);

	log(logERROR) << "Error executing cgi script: " << strerror(errno) ;
	throw CgiException("500");
}

/**
 * execute Method
 *
 * Executes the CGI script and processes its output to create an HTTP response.
 *
 * @param response Reference to a Response object where the CGI output will be stored.
 *
 * This method checks if CGI processing is enabled, extracts the CGI script path and interpreter,
 * creates the environment variables, sets up a pipe, forks the process, and handles the child and parent processes.
 * If any error occurs, it logs the error and throws a CgiException with a 500 status code.
 */
char ** Cgi::createEnv(std::string const &cgiPath)
{
	Request 					*request = this->_request;
	ServerSettings				*serverData = this->_serverData;
	std::vector<std::string>	envVec;

	if (request->getMethod() == GET)
	{
		envVec.push_back("REQUEST_METHOD=GET");
		envVec.push_back("QUERY_STRING=" + findKey(request->getLoc(), "?", ' '));
	}
	else if (request->getMethod() == POST)
	{
		envVec.push_back("REQUEST_METHOD=POST");
		envVec.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
		envVec.push_back("CONTENT_LENGTH=" + toString(request->getContentLen()));
	}
	envVec.push_back("REDIRECT_STATUS=200");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("REQUEST_URI=" + request->getLoc());
	envVec.push_back("HTTP_COOKIE=" + request->getsessionId());

	envVec.push_back("SCRIPT_NAME=" + request->getLoc());
	envVec.push_back("SCRIPT_FILENAME=" + cgiPath);

	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("SERVER_SOFTWARE=Webserv/1.0");
	envVec.push_back("SERVER_PORT=" + toString(serverData->port));
	envVec.push_back("SERVER_ADDR=" + serverData->host);

	return (vectorToCharStarStar(envVec));
}

/**
 * extractCgi Method
 *
 * Determines the path to the CGI script and the interpreter based on the request, location, and server settings.
 *
 * @param cgiScriptPath Reference to a string where the CGI script path will be stored.
 * @param interpreter Reference to a string where the CGI interpreter path will be stored.
 *
 * This method checks if CGI processing is enabled at the location or server level. It extracts the CGI script path
 * and interpreter based on the request's location and the configured CGI settings. If CGI is enabled at the location level,
 * it uses the location's root, CGI bin, and CGI extension to construct the script path and sets the interpreter.
 * If CGI is enabled at the server level, it uses the server's root, CGI bin, and CGI extension to construct the script path
 * and sets the interpreter.
 */
void Cgi::extractCgi(std::string &cgiScriptPath, std::string &interpreter)
{
	Request *request = this->_request;
	if (this->_loc->cgi)
	{
		std::size_t	i = request->getLoc().rfind("/");
		std::size_t	j = request->getLoc().rfind(_loc->cgi_extension);
		std::string cgiFile = request->getLoc().substr(i, j - i + _loc->cgi_extension.length());
		cgiScriptPath = _loc->root + _loc->cgi_bin + cgiFile;
		interpreter = _loc->cgi_pass;
	}
	else if (this->_serverData->cgi)
	{
		std::size_t	i = request->getLoc().rfind("/");
		std::size_t	j = request->getLoc().rfind(_serverData->cgi_extension);
		std::string cgiFile = request->getLoc().substr(i, j - i + _serverData->cgi_extension.length());
		cgiScriptPath = _serverData->root + _serverData->cgi_bin + cgiFile;
		interpreter = _serverData->cgi_pass;
	}
}

/**
 * readCgiOutput Method
 *
 * Reads the output of the CGI script from the pipe and returns it as a string.
 *
 * @param pipefd Pointer to an array of two integers representing the pipe file descriptors.
 * @return A string containing the output of the CGI script.
 *
 * This method reads data from the pipe in chunks and appends it to a string until there is no more data to read.
 * It uses a buffer to read the data and handles the case where the read operation returns zero, indicating the end of the data.
 */
std::string Cgi::readCgiOutput(int *pipefd)
{
	std::string cgiOutput;
	char buffer[512];
	ssize_t bytesRead;
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, bytesRead);

	return cgiOutput;
}

/**
 * createResponse Method
 *
 * Creates an HTTP response based on the CGI script's output.
 *
 * @param response Reference to a Response object where the CGI output will be stored.
 * @param cgiOutput Reference to a string containing the output of the CGI script.
 *
 * This method sets the HTTP status code, body, reason phrase, content type, and connection type
 * for the response based on the CGI output and the request's connection settings. It logs a debug
 * message indicating that the CGI response was created successfully.
 */
void	Cgi::createResponse(Response &response, std::string &cgiOutput)
{
	response.setStatus(200);
	response.setBody(cgiOutput);
	response.setReason("OK");
	response.setType("text/html");
	response.setConnection(this->_request->getConnection());

	log(logDEBUG) << "Created CGI response succesfully";
}

/**
 * getExtension Method
 *
 * Retrieves the CGI script extension based on the location or server settings.
 *
 * @return A string containing the CGI script extension.
 *
 * This method checks if CGI processing is enabled at the location or server level.
 * If CGI is enabled at the location level, it returns the location's CGI extension.
 * If CGI is enabled at the server level, it returns the server's CGI extension.
 * If neither is enabled, it sets the _isTrue flag to false.
 */
std::string	Cgi::getExtension() 
{
	std::string ext;

	if (_loc->cgi)
		ext = _loc->cgi_extension;
	else if (_serverData->cgi)
		ext = _serverData->cgi_extension;
	else
		this->_isTrue = false;

	return ext;
}

/**
 * checkUri Method
 *
 * Checks if the request URI matches the given CGI script extension.
 *
 * @param ext A string containing the CGI script extension to check.
 * @return A boolean value indicating whether the request URI matches the given extension.
 *
 * This method compares the end of the request URI with the given extension. If the URI ends with the extension
 * or contains the extension followed by a query string, it returns true. Otherwise, it returns false.
 */
bool	Cgi::checkUri(std::string ext)
{
	std::size_t	len = ext.length();
	int			finalChars = _request->getLoc().size() - len;

	if (_request->getLoc().length() >= len && _request->getLoc().substr(finalChars) == ext)
		return true;
	else if (_request->getLoc().find(ext + "?") != std::string::npos)
		return true;
	else
		return false;
}