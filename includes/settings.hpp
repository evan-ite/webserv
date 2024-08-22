#ifndef SETTINGS_HPP
# define SETTINGS_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
# include <sstream>
# include <fstream>
# include <ctime>
# include <stdio.h>
# include <stdlib.h>
# include <stack>
# include <map>
# include <vector>
# include <signal.h>
# include <poll.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fstream>
# include <algorithm>
# include <cctype>
# include <sys/wait.h>
# include <dirent.h>
# include <typeinfo>

// Error descriptions
# define E400 "Bad Request"
# define E403 "Forbidden"
# define E404 "Not Found"
# define E405 "Method Not Allowed"
# define E413 "Request Entity Too Large"
# define E415 "Unsupported Media Type"
# define E500 "Internal Server Error"
# define E200 "OK"
# define E201 "Created"
# define E204 "No Content"

// defines
# define MAX_EVENTS 4096
# define BUFFER_SIZE 2048 //read buffer size for sockets
# define CONNECTION_TIMEOUT 1 // in sec
# define HTTPVERSION "HTTP/1.1"
# define MIMEFILE "utils/MIME.txt"
# define LOGLEVEL logINFO
# define DEFAULT_CONF "./default/conf/default.conf"
# define CGI_TIMEOUT 2 // in seconds
# define COOKIE_LIFETIME 300 // in seconds

// enums
enum HttpMethod
{
	GET,
	POST,
	DELETE,
	INVALID
};

// local header files
# include "ASetting.hpp"
# include "AHeader.hpp"
# include "Cookie.hpp"
# include "Client.hpp"
# include "Location.hpp"
# include "Request.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "Logger.hpp"
# include "Response.hpp"
# include "Webserv.hpp"
# include "Cgi.hpp"

extern int	g_signal;

std::string	findKey(std::string file_str, std::string keyword, char separator);
std::string	splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string	readFileToString(const std::string& filename);
std::string	checkMime(const std::string &extension);
std::string	findType(const std::string &filename);
std::string	getDateTime();
std::string	toString(int value);
char**		vectorToCharStarStar(const std::vector<std::string>& vec);
int			makeNonBlocking(int fd);
std::string	generateRandomString(int length);
void		removeCharacter(std::string& str, char charToRemove);
std::string	trimLeadingWhitespace(const std::string& str);
bool		startsWith(const std::string& str, const std::string& prefix);

#endif
