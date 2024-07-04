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
# include "Response.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include "Webserv.hpp"

# define MAX_EVENTS 1024
# define HTTPVERSION "HTTP/1.1"
# define MIMEFILE "MIME.txt"
# define LOGLEVEL logDEBUG
# define DEFAULT_CONF "./default.conf"

extern int g_signal;

std::string	findKey(std::string file_str, std::string keyword, char separator);
std::string	splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string	readFileToString(const std::string& filename);
int			make_socket_non_blocking(int sfd);

#endif
