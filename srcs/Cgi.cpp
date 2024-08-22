#include "../includes/settings.hpp"

/**
 * @brief Constructs a Cgi object with the given request and location.
 *
 * This constructor initializes a Cgi object by setting the request and location
 * members to the provided arguments. It also initializes the _env member to NULL.
 *
 * @param request The HTTP request to be processed by the CGI.
 * @param loc The location configuration associated with the request.
 */
Cgi::Cgi(Request &request, Location *loc) : _request(request), _loc(loc)
{
	_env = NULL;
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
	{
		if (_env) {
			for (int i = 0; _env[i] != NULL; ++i)
				delete[] _env[i];
			delete[] _env;
		}
		_env = NULL;
	}
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
	log(logDEBUG) << "Starting CGI";
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
 * @brief Executes the CGI script in the child process.
 *
 * This function is called in the child process to execute the CGI script. It sets an alarm
 * to enforce a timeout for the CGI execution. If the request method is POST, it writes the
 * request body to the pipe. It then duplicates the pipe file descriptors to standard input
 * and output, closes the original pipe file descriptors, and executes the CGI script using
 * the specified interpreter and script path. If execve fails, it logs an error message and
 * throws a CgiException with a "500" status code.
 *
 * @param pipefd An array of two integers representing the pipe file descriptors.
 * @param cgiPath The path to the CGI script to be executed.
 * @param interpreter The interpreter to be used for executing the CGI script.
 * @throws CgiException if execve fails.
 */
void	Cgi::executeCgiChild(int *pipefd, std::string cgiPath, std::string interpreter)
{
	alarm(CGI_TIMEOUT);

	if (_request.getMethod() == POST)
		write(pipefd[1], _request.getBody().data(), _request.getBody().size());

	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);

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
	std::vector<std::string>	envVec;

	if (_request.getMethod() == GET)
	{
		envVec.push_back("REQUEST_METHOD=GET");
		envVec.push_back("QUERY_STRING=" + findKey(_request.getUri(), "?", ' '));
	}
	else if (_request.getMethod() == POST)
	{
		envVec.push_back("REQUEST_METHOD=POST");
		envVec.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
		envVec.push_back("CONTENT_LENGTH=" + toString(_request.getContentLen()));
	}
	envVec.push_back("REDIRECT_STATUS=200");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("REQUEST_URI=" + _request.getUri());
	envVec.push_back("HTTP_COOKIE=" + _request.getSessionId());

	envVec.push_back("SCRIPT_NAME=" + _request.getUri());
	envVec.push_back("SCRIPT_FILENAME=" + cgiPath);

	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("SERVER_SOFTWARE=Webserv/1.0");

	return (vectorToCharStarStar(envVec));
}

/**
 * @brief Extracts the CGI script path and interpreter from the request URI.
 *
 * This function determines the path to the CGI script and the interpreter to be used
 * based on the request URI and the location configuration. It extracts the relevant
 * portion of the URI that corresponds to the CGI script and constructs the full path
 * to the script by combining the root directory, CGI bin directory, and the extracted
 * CGI file name. It also retrieves the interpreter path from the location configuration.
 *
 * @param cgiScriptPath A reference to a string where the full path to the CGI script will be stored.
 * @param interpreter A reference to a string where the interpreter path will be stored.
 */
void Cgi::extractCgi(std::string &cgiScriptPath, std::string &interpreter)
{
    std::size_t	lastSlash = _request.getUri().rfind("/");
    std::size_t	cgiExtIndex = _request.getUri().rfind(_loc->getCgiExtension());
    std::string cgiFile = _request.getUri().substr(lastSlash, cgiExtIndex - lastSlash + _loc->getCgiExtension().length());
    cgiScriptPath = _loc->getRoot() + _loc->getCgiBin() + cgiFile;
    interpreter = _loc->getCgiPass();
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
	response.setContentType("text/html");
	response.setConnection(_request.getConnection());

	log(logDEBUG) << "Created CGI response succesfully";
}
