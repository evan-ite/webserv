#include "../includes/settings.hpp"

// Constructors
Cgi::Cgi():  _request(NULL), _serverData(NULL), _isTrue(false)
{}

Cgi::Cgi(Request *request, ServerSettings *serverData)
{
	std::string ext = serverData->cgi_extension; // default ".cgi"
	std::size_t	len = ext.length();

	if (((request->getLoc().length() >= len
		&& request->getLoc().substr(request->getLoc().size() - len) == ext)
		|| request->getLoc().find(ext + "?") != std::string::npos)
		&& serverData->cgi) {
			this->_isTrue = true;
			this->_request = request;
			this->_serverData = serverData;
		}
	else {
		this->_isTrue = false;
		this->_request = NULL;
		this->_serverData = NULL;
	}
}


Cgi::Cgi(const Cgi &copy)
{
	(void) copy;
}


// Destructor
Cgi::~Cgi()
{
}


// Operators
Cgi & Cgi::operator=(const Cgi &assign)
{
	(void) assign;
	return *this;
}

bool	Cgi::isTrue() {
	return this->_isTrue;
}

void	Cgi::execute(Response &response) {
	Request *request = this->_request;
	ServerSettings *serverData = this->_serverData;

	if (!this->_isTrue)
		return ;
	try
	{
		log(logINFO) << "Using CGI to fetch data";
		// Determine the path to the CGI script based on the request and serverData
		std::size_t					i = request->getLoc().rfind("/");
		std::size_t					j = request->getLoc().rfind(serverData->cgi_extension);
		std::string					cgiFile = request->getLoc().substr(i, j - i + serverData->cgi_extension.length());
		std::string					cgiScriptPath = serverData->root + serverData->cgi_bin + cgiFile;

		char **env = createEnv(cgiScriptPath, cgiFile);

		// Prepare to capture the CGI script's output
		int pipefd[2];
		if (pipe(pipefd) == -1) {
			log(logERROR) << "Error creating pipe";
			throw CgiException();
		}

		pid_t pid = fork();
		if (pid == -1) {
			log(logERROR) << "Error forking";
			throw CgiException();
		}
		else if (pid == 0) { // Child process: execute the CGI script

			// If POST, write the data to the pipe
			if (request->getMethod() == POST) {
				write(pipefd[1], request->getBody().data(), request->getBody().size());
			}

			dup2(pipefd[0], STDIN_FILENO);	// Redirect stdin to the pipe
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
			close(pipefd[1]);

			// Prepare arguments for execve
			char *args[] = { strdup(cgiScriptPath.c_str()), NULL };

			// Convert environment variables to char* array
			execve(cgiScriptPath.c_str(), args, env);

			log(logERROR) << "Error executing cgi script: " << strerror(errno) ;
			throw CgiException();
		}
		else
		{
			waitpid(pid, NULL, 0);
			log(logDEBUG) << "Back in parent process";
			// Parent process: read the CGI script's output
			close(pipefd[1]);  // Close write end of the pipe

			std::string cgiOutput;
			char buffer[512];
			ssize_t bytesRead;
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
				cgiOutput.append(buffer, bytesRead);
			close(pipefd[0]);

			// Process the CGI output and create the HTTP response
			if (!cgiOutput.empty()) {
				response.setStatus(200);
				response.setBody(cgiOutput);
				response.setReason("OK");
				response.setType("text/html");  // Adjust based on CGI output
				response.setConnection("keep-alive");  // Close the connection after handling request
			} else { throw CgiException(); }
		}
	} catch (CgiException &e) {
		// Handle case where CGI script produces no output
		log(logERROR) << "CGI output empty";
		response.setStatus(500);
		response.setBody(readFileToString("content/error/500.html"));
		response.setReason("Internal Server Error");
		response.setType("text/html");
		response.setConnection("keep-alive");
	}

}

char ** Cgi::createEnv(std::string const &cgiPath, std::string const &cgiFile)
{
	Request *request = this->_request;
	ServerSettings *serverData = this->_serverData;
	std::vector<std::string>	envVec;

	// Set up environment variables specific to GET or POST
	if (request->getMethod() == GET) {
		envVec.push_back("REQUEST_METHOD=GET");
		envVec.push_back("QUERY_STRING=" + findKey(request->getLoc(), "?", ' '));
	} else if (request->getMethod() == POST) {
		envVec.push_back("REQUEST_METHOD=POST");
		envVec.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
		envVec.push_back("CONTENT_LENGTH=" + toString(request->getContentLen()));
	}
	// Set up common environment variables required by the CGI script
	envVec.push_back("REDIRECT_STATUS=200");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVec.push_back("REQUEST_URI=" + request->getLoc());

	envVec.push_back("SCRIPT_NAME=" + cgiPath);
	envVec.push_back("SCRIPT_FILENAME=" + cgiFile);
	// envVec.push_back("PATH_INFO=");

	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("SERVER_SOFTWARE=Webserv/1.0");
	envVec.push_back("SERVER_PORT=" + toString(serverData->port));
	envVec.push_back("SERVER_NAME=" + serverData->server_name);
	envVec.push_back("SERVER_ADDR=" + serverData->host);

	// envVec.push_back("REMOTE_ADDR=");
	// envVec.push_back("REMOTE_IDENT=");
	// envVec.push_back("REMOTE_USER=");

	// for (size_t i = 0; i < envVec.size(); ++i) {
	// 	std::cout << envVec[i] << std::endl;
	// }

	return vectorToCharStarStar(envVec);
}
