#include "../includes/server.hpp"

// Constructors
Cgi::Cgi():  _request(NULL), _serverData(NULL), _isTrue(false)
{}

Cgi::Cgi(Request *request, Server *serverData) 
{
	std::string ext = (*serverData).cgi_extension; // default ".cgi"
	std::size_t	len = ext.length();

	if ((((*request)._location.length() >= len
		&& (*request)._location.substr((*request)._location.size() - len) == ext) 
		|| ((*request)._location.find(ext + "?") != std::string::npos))
		&& (*serverData).cgi) {
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
	Request request = *this->_request;
	Server serverData = *this->_serverData;

	if (!this->_isTrue)
		return ;
	try 
	{
		log(logINFO) << "Using CGI to fetch data";
		// Determine the path to the CGI script based on the request and serverData
		std::size_t					i = request._location.rfind("/");
		std::string					cgiScript = request._location.substr(i);
		std::string					cgiScriptPath = serverData.root + serverData.cgi_bin + cgiScript;
		std::vector<std::string>	envVec;

		log(logDEBUG) << "this is the body now!!: \n" << request._body;
		// Set up environment variables specific to GET or POST
		if (request._method == GET) {
			envVec.push_back("REQUEST_METHOD=GET");
			envVec.push_back("QUERY_STRING=" + findKey(request._location, "?", ' '));
		} else if (request._method == POST) {
			envVec.push_back("REQUEST_METHOD=POST");
			envVec.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
			envVec.push_back("CONTENT_LENGTH=" + to_string(request._contentLenght)); 
		}
		// Set up common environment variables required by the CGI script
		envVec.push_back("SCRIPT_NAME=" + cgiScriptPath);
		envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envVec.push_back("SERVER_SOFTWARE=MyServer/1.0");

		for (size_t i = 0; i < envVec.size(); ++i) {
			std::cout << envVec[i] << std::endl;
		}

		//char **env = vectorToCharStarStar(envVec);

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
		else if (pid == 0) {
			// Child process: execute the CGI script
			// If POST, write the data to the pipe
			if (request._method == POST) {
				write(pipefd[1], request._body.data(), request._body.size());
			}

			dup2(pipefd[0], STDIN_FILENO);
			dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
			// dup2(pipefd[1], STDERR_FILENO);  // Redirect stderr to the pipe
			close(pipefd[0]);  // Close read end of the pipe
			close(pipefd[1]);  // Close write end of the pipe
			
			// Prepare arguments for execve
			char *args[] = { strdup(cgiScriptPath.c_str()), NULL };

			extern char **environ;
			// Convert environment variables to char* array
			execve(cgiScriptPath.c_str(), args, environ);

			log(logERROR) << "Error executing cgi script: " << strerror(errno) ;
			throw CgiException();
		} 
		else 
		{
			log(logDEBUG) << "Back in parent process";
			// Parent process: read the CGI script's output
			close(pipefd[1]);  // Close write end of the pipe

			std::string cgiOutput;
			char buffer[512];
			ssize_t bytesRead;
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
				cgiOutput.append(buffer, bytesRead);
				// log(logDEBUG) << "reading buffer... \n output: " << cgiOutput;
			}
			close(pipefd[0]);
			waitpid(pid, NULL, 0);

			log(logDEBUG) << "pipes closed";

			// Process the CGI output and create the HTTP response
			if (!cgiOutput.empty()) {
				response.setStatus(200);
				response.setBody(cgiOutput);
				response.setReason("OK");
				response.setType("text/html");  // Adjust based on CGI output
				response.setConnection("close");  // Close the connection after handling request
			} else { throw CgiException(); }
		} 
	} catch (CgiException &e) {
		// Handle case where CGI script produces no output
		log(logERROR) << "CGI output empty";
		response.setStatus(500);
		response.setBody(readFileToString("content/error/500.html"));
		response.setReason("Internal Server Error");
		response.setType("text/html");
		response.setConnection("close");
	}

}
