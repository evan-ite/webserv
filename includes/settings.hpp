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
# include <sys/epoll.h>
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
// defines
# define MAX_EVENTS 1024
# define BUFFER_SIZE 1024 //read buffer size for sockets
# define CONNECTION_TIMEOUT 1 // in sec
# define HTTPVERSION "HTTP/1.1" // are we using this?
# define MIMEFILE "utils/MIME.txt"
# define LOGLEVEL logDEBUG
# define UPLOAD_DIR "upload/"
# define DEFAULT_CONF "./conf/default.conf"
# define FALLBACK_CONF "./conf/fallback.conf"
# define TEMPLATE   "./content/html/template2.html"
# define TOOLARGE "413"
// local header files
# include "Config.hpp"
# include "Client.hpp"
# include "Server.hpp"
# include "Logger.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Webserv.hpp"
# include "Cgi.hpp"
# include "htmlTemplates.hpp"

extern int	g_signal;

std::string					findKey(std::string file_str, std::string keyword, char separator);
std::string					splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string					readFileToString(const std::string& filename);
std::string					checkMime(const std::string &extension);
std::string					findType(const std::string &filename);
std::string					getDateTime();
std::string					toString(int value);
char**						vectorToCharStarStar(const std::vector<std::string>& vec);
int							makeNonBlocking(int fd);

#endif
