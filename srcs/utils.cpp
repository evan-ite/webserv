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
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		log(logERROR) << "Error opening file: " << filename;
		return "";
	}

	// Get file size
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Read the file into a vector<char>
	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	file.close();

	// Convert buffer to std::string
	return std::string(buffer.data(), buffer.size());
}

// mostly chatgpt but I do think I understand what it does
/* Takes a socket filedescriptor as argument and sets it to
non-blocking mode. Returns 0 on success, -1 on error. */
int make_socket_non_blocking(int sfd)
{
	int flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (-1);
	}
	flags |= O_NONBLOCK;
	if (fcntl(sfd, F_SETFL, flags) == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (-1);
	}
	return 0;
}

/* Returns date and time in astring formatted for the HTTP response */
std::string getDateTime()
{
	std::time_t raw_time;
	std::time(&raw_time);
	struct std::tm *gmt_time = std::gmtime(&raw_time);
	char buffer[30];
	std::strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
	return std::string(buffer);
}