#include "../includes/settings.hpp"

// Constructors
Cgi::Cgi(Request &request, Location &loc)
{
	this->_env = NULL;
	std::string ext = loc.getCgiExtension();
	std::size_t	len = ext.length();

	if (((request.getPath().length() >= len
		&& request.getPath().substr(request.getPath().size() - len) == ext)
		|| request.getPath().find(ext + "?") != std::string::npos))
	{
		this->_isTrue = true;
		this->_loc = loc;
	}
	else
		this->_isTrue = false;
}

Cgi::Cgi(const Cgi &copy)
{
	(void) copy;
}

// Destructor
Cgi::~Cgi()
{
	if (this->_env)
		delete this->_env;
}


// Operators
Cgi & Cgi::operator=(const Cgi &assign)
{
	(void) assign;
	return (*this);
}

bool	Cgi::isTrue()
{
	return (this->_isTrue);
}

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
		throw CgiException("400");
	}
	this->_env = createEnv(cgiScriptPath);

	// Prepare to capture the CGI script's output
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
	else if (pid == 0) // Child process: execute the CGI script
		this->executeCgiChild(pipefd, cgiScriptPath, interpreter);
	else
	{
		int status;
		waitpid(pid, &status, 0);
		if (!WIFEXITED(status) && WEXITSTATUS(status))
			throw CgiException("500");
		log(logDEBUG) << "Back in parent process";
		close(pipefd[1]);
		// read the CGI script's output
		std::string cgiOutput = this->readCgiOutput(pipefd);
		close(pipefd[0]);

		// Process the CGI output and create the HTTP response
		if (!cgiOutput.empty())
			this->createResponse(response, cgiOutput);
		else
			throw CgiException("500");
	}
}

void	Cgi::executeCgiChild(int *pipefd, std::string cgiPath, std::string interpreter)
{
	Request request = this->_request;
	// If POST, write the data to the pipe
	if (request.getMethod() == POST)
		write(pipefd[1], request.getBody().data(), request.getBody().size());

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

/* Create an environment for the CGI script */
char ** Cgi::createEnv(std::string const &cgiPath)
{
	Request 					request = this->_request;
	std::vector<std::string>	envVec;

	// Set up environment variables specific to GET or POST
	if (request.getMethod() == GET)
	{
		envVec.push_back("REQUEST_METHOD=GET");
		envVec.push_back("QUERY_STRING=" + findKey(request.getPath(), "?", ' '));
	}
	else if (request.getMethod() == POST)
	{
		envVec.push_back("REQUEST_METHOD=POST");
		envVec.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
		envVec.push_back("CONTENT_LENGTH=" + toString(request.getContentLen()));
	}
	// Set up common environment variables required by the CGI script
	envVec.push_back("REDIRECT_STATUS=200");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("REQUEST_URI=" + request.getPath());
	envVec.push_back("HTTP_COOKIE=" + request.getsessionId());
	envVec.push_back("SCRIPT_NAME=" + request.getPath()); // rel path
	envVec.push_back("SCRIPT_FILENAME=" + cgiPath); // path incl root
	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("SERVER_SOFTWARE=Webserv/1.0");
	return (vectorToCharStarStar(envVec));
}

 /* Determine the path to the CGI script based on the request, location and server.
Store the found strings in the arguments passed by reference. */
void Cgi::extractCgi(std::string &cgiScriptPath, std::string &interpreter)
{
	Request request = this->_request;
	std::size_t	i = request.getPath().rfind("/");
	std::size_t	j = request.getPath().rfind(this->_loc.getCgiExtension());
	std::string cgiFile = request.getPath().substr(i, j - i + this->_loc.getCgiExtension().length());
	cgiScriptPath = this->_loc.getRoot() + this->_loc.getCgiBin() + cgiFile;
	interpreter = this->_loc.getCgiPass();
}

/* Read the output of the CGI script from the pipe
and return it as a string */
std::string Cgi::readCgiOutput(int *pipefd)
{
	std::string cgiOutput;
	char buffer[512];
	ssize_t bytesRead;
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, bytesRead);

	return cgiOutput;
}

void	Cgi::createResponse(Response &response, std::string &cgiOutput)
{
	response.setStatus(200);
	response.setBody(cgiOutput);
	response.setReason("OK");
	response.setType("text/html");
	response.setConnection(this->_request.getConnection());

	log(logDEBUG) << "Created CGI response succesfully";
}
