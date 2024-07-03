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
# include "Response.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "Logger.hpp"

# define HTTPVERSION "HTTP/1.1"
# define LOGLEVEL logDEBUG

std::string findKey(std::string file_str, std::string keyword, char separator);
std::string splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string readFileToString(const std::string& filename);

#endif
