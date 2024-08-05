#include "../includes/settings.hpp"

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

/**
 * @brief Finds the value associated with a keyword in a string.
 *
 * This function searches for a keyword in the provided string and returns the value
 * associated with it, separated by the specified separator.
 *
 * @param file_str The string to search within.
 * @param keyword The keyword to search for.
 * @param separator The character that separates the keyword and its value.
 * @return The value associated with the keyword, or an empty string if the keyword is not found.
 */
std::string findKey(std::string file_str, std::string keyword, char separator)
{
	std::string::size_type start_pos = file_str.find(keyword);

	if (start_pos == std::string::npos)
		return ("");

	start_pos += keyword.length();
	std::string::size_type end_pos = file_str.find(separator, start_pos);

	if (end_pos == std::string::npos)
		return (file_str.substr(start_pos));

	return (file_str.substr(start_pos, end_pos - start_pos));
}

/**
 * @brief Splits a string by a delimiter and returns the first part.
 *
 * This function splits the provided string by the specified delimiter and returns
 * the first part of the split string.
 *
 * @param str The string to split.
 * @param delimiter The delimiter to split the string by.
 * @return The first part of the split string.
 */
std::string splitReturnFirst(const std::string& str, const std::string& delimiter)
{
	size_t pos = str.find(delimiter);
	if (pos == std::string::npos)
		return (str);
	else
		return (str.substr(0, pos));
}

/**
 * @brief Reads the content of a file into a string.
 *
 * This function takes a filename (including path) as an argument and returns
 * the content of the file as a string.
 *
 * @param filename The name of the file to read.
 * @return The content of the file as a string, or an empty string if the file cannot be opened.
 */
std::string readFileToString(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		log(logERROR) << "Error opening file: " << filename;
		return ("");
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
	return (std::string(buffer.data(), buffer.size()));
}

/**
 * @brief Sets a socket to non-blocking mode.
 *
 * This function takes a socket file descriptor as an argument and sets it to
 * non-blocking mode. Returns 0 on success, -1 on error.
 *
 * @param sfd The socket file descriptor.
 * @return 0 on success, -1 on error.
 */
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
	return (0);
}

/**
 * @brief Returns the current date and time formatted for an HTTP response.
 *
 * This function returns the current date and time as a string formatted
 * according to the HTTP date/time specification.
 *
 * @return The current date and time as a string.
 */
std::string getDateTime()
{
	std::time_t raw_time;
	std::time(&raw_time);
	struct std::tm *gmt_time = std::gmtime(&raw_time);
	char buffer[30];
	std::strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
	return (std::string(buffer));
}

/**
 * @brief Converts an integer to a string.
 *
 * This function takes an integer value and converts it to a string.
 *
 * @param value The integer value to convert.
 * @return The integer value as a string.
 */
std::string toString(int value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

/**
 * @brief Converts a vector of strings to an array of C-style strings.
 *
 * This function takes a vector of strings and converts it to an array of
 * C-style strings (char**). The array is null-terminated.
 *
 * @param vec The vector of strings to convert.
 * @return A pointer to the array of C-style strings.
 */
char** vectorToCharStarStar(const std::vector<std::string>& vec)
{
	int numElements = vec.size();

	// Allocate memory for an array of char* pointers
	char** charArray = new char*[numElements + 1];

	// Copy each string from the vector into the char* array
	for (int i = 0; i < numElements; ++i)
	{
		// Allocate memory for each string and copy it
		charArray[i] = new char[vec[i].length() + 1];  // +1 for null terminator
		std::strcpy(charArray[i], vec[i].c_str());
	}

	charArray[numElements] = NULL;

	return (charArray);
}

/**
 * @brief Sets a file descriptor to non-blocking mode.
 *
 * This function takes a file descriptor as an argument and sets it to
 * non-blocking mode. Returns 1 on success, 0 on error.
 *
 * @param fd The file descriptor.
 * @return 1 on success, 0 on error.
 */
int	makeNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (0);
	}
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
	{
		log(logERROR) << "critical fcntl error";
		return (0);
	}
	return (1);
}

/**
 * @brief Generates a pseudorandom alphanumeric string of a specified length.
 *
 * This function generates a pseudorandom alphanumeric string of the specified length.
 *
 * @param length The length of the generated string.
 * @return The generated pseudorandom alphanumeric string.
 */
std::string generateRandomString(int length)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string result;
	result.reserve(length);

	for (int i = 0; i < length; ++i)
		result += charset[rand() % (sizeof(charset) - 1)];

	return (result);
}

/**
 * @brief Removes all occurrences of a substring from a string.
 *
 * This function takes a string and a substring, and removes all occurrences
 * of the substring from the string, returning the resulting string.
 *
 * @param str The original string.
 * @param substr The substring to remove.
 * @return The resulting string with all occurrences of the substring removed.
 */
std::string removeSubstr(const std::string& str, const std::string& substr)
{
	std::string	result = str;
	std::string::size_type pos = str.find(substr);

	while (pos != std::string::npos)
	{
		result.erase(pos, substr.length());
		pos = result.find(substr);
	}

	return (result);
}
