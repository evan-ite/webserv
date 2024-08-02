#include "../includes/settings.hpp"

// Constructors
Cgi::Cgi(): _isTrue(false) {}

Cgi::Cgi(Request *request, ServerSettings *serverData, Location *loc)
{
	this->_env = NULL;
	std::string ext;

	if (loc->cgi)
		ext = loc->cgi_extension;
	else if (serverData->cgi)
		ext = serverData->cgi_extension;
	else {
		this->_isTrue = false;
		return ;
	}

	std::size_t	len = ext.length();

	if (((request->getLoc().length() >= len
		&& request->getLoc().substr(request->getLoc().size() - len) == ext)
		|| request->getLoc().find(ext + "?") != std::string::npos))
	{
		this->_isTrue = true;
		this->_request = request;
		this->_serverData = serverData;
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

	std::string cgiFile;
	std::string cgiScriptPath;
	this->extractCgi(cgiFile, cgiScriptPath);
	this->_env = createEnv(cgiScriptPath, cgiFile);

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
		this->executeCgiChild(pipefd, cgiScriptPath);
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

void	Cgi::executeCgiChild(int *pipefd, std::string cgiScriptPath)
{
	Request *request = this->_request;
	// If POST, write the data to the pipe
	if (request->getMethod() == POST)
		write(pipefd[1], request->getBody().data(), request->getBody().size());

	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);

	char *args[] = { strdup(cgiScriptPath.c_str()), NULL };
	execve(cgiScriptPath.c_str(), args, this->_env);

	log(logERROR) << "Error executing cgi script: " << strerror(errno) ;
	throw CgiException("500");
}

/* Create an environment for the CGI script */
char ** Cgi::createEnv(std::string const &cgiPath, std::string const &cgiFile)
{
	Request 					*request = this->_request;
	ServerSettings				*serverData = this->_serverData;
	std::vector<std::string>	envVec;

	// Set up environment variables specific to GET or POST
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
	// Set up common environment variables required by the CGI script
	envVec.push_back("REDIRECT_STATUS=200");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("REQUEST_URI=" + request->getLoc());
	envVec.push_back("HTTP_COOKIE=" + request->getsessionId());

	envVec.push_back("SCRIPT_NAME=" + cgiPath);
	envVec.push_back("SCRIPT_FILENAME=" + cgiFile);

	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("SERVER_SOFTWARE=Webserv/1.0");
	envVec.push_back("SERVER_PORT=" + toString(serverData->port));
	envVec.push_back("SERVER_ADDR=" + serverData->host);

	return (vectorToCharStarStar(envVec));
}

 /* Determine the path to the CGI script based on the request, location and server.
Store the found strings in the arguments passed by reference. */
void Cgi::extractCgi(std::string &cgiFile, std::string &cgiScriptPath)
{
	Request *request = this->_request;
	if (this->_loc->cgi)
	{
		std::size_t	i = request->getLoc().rfind("/");
		std::size_t	j = request->getLoc().rfind(_loc->cgi_extension);
		cgiFile = request->getLoc().substr(i, j - i + _loc->cgi_extension.length());
		cgiScriptPath = _loc->root + _loc->cgi_bin + cgiFile;
	}
	else if (this->_serverData->cgi)
	{
		std::size_t	i = request->getLoc().rfind("/");
		std::size_t	j = request->getLoc().rfind(_serverData->cgi_extension);
		cgiFile = request->getLoc().substr(i, j - i + _serverData->cgi_extension.length());
		cgiScriptPath = _serverData->root + _serverData->cgi_bin + cgiFile;
	}
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
	response.setConnection(this->_request->getConnection());

	log(logDEBUG) << "Created CGI response succesfully";
}
