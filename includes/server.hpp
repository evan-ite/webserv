#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
# include <sstream>
# include <fstream>
# include <ctime>
# include <stdio.h>
# include <stdlib.h>
# include <map>
# include <vector>
# include <signal.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
// local header files
# include "Config.hpp"
# include "Logger.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Webserv.hpp"
# include "Parser.hpp"

# define MAX_EVENTS 1024
# define HTTPVERSION "HTTP/1.1"
# define MIMEFILE "utils/MIME.txt"
# define LOGLEVEL logDEBUG
# define DEFAULT_CONF "./conf/default.conf"
# define SAMPLE_RES "HTTP/1.1 200 OK \
Date: Fri, 05 Jul 2024 12:00:00 GMT \
Server: Apache/2.4.41 (Ubuntu) \
Last-Modified: Fri, 05 Jul 2024 11:00:00 GMT \
Content-Length: 138 \
Content-Type: text/html; charset=UTF-8 \
Connection: keep-alive \
 \
<!DOCTYPE html> \
<html lang=\"en\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <title>Sample Page</title> \
</head> \
<body> \
    <h1>Hello, World!</h1> \
    <p>This is a sample HTML page.</p> \
</body> \
</html> \
"

extern int g_signal;

std::string	findKey(std::string file_str, std::string keyword, char separator);
std::string	splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string	readFileToString(const std::string& filename);
int			make_socket_non_blocking(int sfd);
std::string	checkMime(const std::string &extension);
std::string	findType(const std::string &filename);
std::string getDateTime();


#endif
