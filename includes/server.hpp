#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
# include <sstream>
# include <ctime>
# include "../includes/Response.hpp"

# define HTTPVERSION "HTTP/1.1"

std::string find_key(std::string file_str, std::string keyword, char separator);

#endif
