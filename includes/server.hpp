#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
# include <sstream>
# include <fstream>
# include <ctime>
# include "../includes/Response.hpp"
# include "../includes/Response.hpp"
# include "../includes/Request.hpp"
# include "../includes/Logger.hpp"


# define HTTPVERSION "HTTP/1.1"

std::string findKey(std::string file_str, std::string keyword, char separator);
std::string splitReturnFirst(const std::string& str, const std::string& delimiter);
std::string readFileToString(const std::string& filename);

#endif
