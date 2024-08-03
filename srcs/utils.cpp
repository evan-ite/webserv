#include "../includes/settings.hpp"

/**
 * Finds a key in a given string and returns the value associated with it.
 *
 * @param file_str The string to search for the key.
 * @param keyword The key to search for.
 * @param separator The character that separates the key and value.
 * @return The value associated with the key, or an empty string if the key is not found.
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
 * Splits a string by a delimiter and returns the first part.
 *
 * @param str The string to split.
 * @param delimiter The delimiter to split the string by.
 * @return The first part of the string before the delimiter. If the delimiter is not found, the entire string is returned.
 */
std::string splitReturnFirst(const std::string& str, const std::string& delimiter)
{
	size_t pos = str.find(delimiter);
	if (pos == std::string::npos)
		return (str);
	else
		return (str.substr(0, pos));
}

/* Takes a filename (incl path) as argument and returns
the content of the file as a string */
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

/* Returns date and time in astring formatted for the HTTP response */
std::string getDateTime()
{
	std::time_t raw_time;
	std::time(&raw_time);
	struct std::tm *gmt_time = std::gmtime(&raw_time);
	char buffer[30];
	std::strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
	return (std::string(buffer));
}

/* Int to string */
std::string toString(int value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

// Function to convert std::vector<std::string> to char**
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
 * Makes a file descriptor non-blocking.
 *
 * @param fd The file descriptor to make non-blocking.
 * @return 1 if the file descriptor was successfully made non-blocking, 0 otherwise.
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

/* Takes a string, splits it on delim and returns a vector of std::string tokens */
std::vector<std::string> split(const std::string& str, char del) // are we using this?
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(del);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(del, start);
	}
	tokens.push_back(str.substr(start));
	return (tokens);
}


/**
 * Generates a pseudorandom string of the specified length.
 *
 * @param length The length of the random string to generate.
 * @return The randomly generated string.
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
 * @brief Removes all occurrences of a substring from a given string.
 *
 * @param str The original string.
 * @param substr The substring to be removed.
 * @return The modified string with all occurrences of the substring removed.
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

/**
 * @brief Removes all occurrences of a specified character from a string.
 *
 * This function removes all occurrences of the specified character `charToRemove` from the input string `str`.
 *
 * @param str The string from which to remove the character.
 * @param charToRemove The character to be removed from the string.
 */
void removeCharacter(std::string& str, char charToRemove)
{
	for (std::string::size_type i = 0; i < str.size(); )
	{
		if (str[i] == charToRemove)
			str.erase(i, 1);
		else
			++i;
	}
}

std::ostream& operator<<(std::ostream& os, Location& loc)
{
	os << "Location Path: " << loc.getPath() << "\n";
	os << "Location Root: " << loc.getRoot() << "\n";
	os << "Location Index: " << loc.getIndex() << "\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Server& server)
{
	os << "Server Host: " << server.getHost() << "\n";
	os << "Server Port: " << server.getPort() << "\n";
	os << "Server FD: " << server.getFd() << "\n";
	return os;
}
