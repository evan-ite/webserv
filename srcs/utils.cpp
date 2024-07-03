#include "../includes/server.hpp"

/*
 file_str = "HTTP/1.1 200 OK
			Date: Thu, 27 Jun 2024 12:00:00 GMT
			Content-Type: text/html; charset=UTF-8
			Content-Length: 1256
			Connection: keep-alive "
 keyword = Date:
 seperator = \n
 return value is " Thu, 27 Jun 2024 12:00:00 GMT"
*/
std::string findKey(std::string file_str, std::string keyword, char separator)
{
    std::string::size_type start_pos = file_str.find(keyword);

    if (start_pos == std::string::npos)
        return "";

    start_pos += keyword.length();
    std::string::size_type end_pos = file_str.find(separator, start_pos);

    if (end_pos == std::string::npos)
        return file_str.substr(start_pos);

    return file_str.substr(start_pos, end_pos - start_pos);
}

std::string splitReturnFirst(const std::string& str, const std::string& delimiter)
{
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos)
        return str;
	else
        return str.substr(0, pos);
}

/* Takes a filename (incl path) as argument and returns
the content of the file as a string */
std::string readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        log(logERROR) << "Error opening file: " << filename;
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    file.close();

    return oss.str();
}
