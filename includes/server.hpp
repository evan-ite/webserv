#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
//# include "Response.hpp"

# define HTTPVERSION "HTTP/1.1"

std::string findKey(std::string file_str, std::string keyword, char separator);
std::string splitReturnFirst(const std::string& str, const std::string& delimiter);

#endif
