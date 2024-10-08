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

/**
 * @brief Reads the content of a file and returns it as a std::string.
 *
 * This function takes a filename (including its path) as an argument, opens the file,
 * and reads its content into a std::string. If the file cannot be opened, it logs an
 * error message and returns an empty string.
 *
 * The function first opens the file in binary mode. It then determines the file size
 * by seeking to the end of the file and using tellg(). The file content is read into
 * a std::vector<char> buffer, which is subsequently converted to a std::string and returned.
 *
 * @param filename The name (and path) of the file to be read.
 * @return A std::string containing the content of the file, or an empty string if the file cannot be opened.
 */
std::string readFileToString(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		log(logERROR) << "Error opening file: " << filename;
		return ("");
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	file.close();

	return (std::string(buffer.data(), buffer.size()));
}

/**
 * @brief Gets the current date and time in GMT as a formatted string.
 *
 * This function retrieves the current system time, converts it to GMT (UTC),
 * and formats it as a string according to the HTTP-date format specified in
 * RFC 7231. The format used is: "Day, DD Mon YYYY HH:MM:SS GMT".
 *
 * @return A std::string representing the current date and time in GMT.
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
 * @brief Converts an integer to a std::string.
 *
 * This function takes an integer value and converts it to its string representation
 *
 * @param value The integer value to be converted to a string.
 * @return A std::string representing the integer value.
 */
std::string toString(int value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

/**
 * @brief Converts a std::vector<std::string> to a char** array.
 *
 * This function takes a vector of strings and converts it into an array of C-style
 * strings (char**). It allocates memory for each string and copies the content
 * from the vector to the newly allocated memory. The last element of the array
 * is set to NULL to indicate the end of the array.
 *
 * @param vec The input vector of strings to be converted.
 * @return A char** array containing the C-style strings.
 */
char** vectorToCharStarStar(const std::vector<std::string>& vec)
{
	int numElements = vec.size();
	char** charArray = new char*[numElements + 1];

	for (int i = 0; i < numElements; ++i)
	{
		charArray[i] = new char[vec[i].length() + 1];
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

/**
 * Overloaded insertion operator for printing the Location object.
 *
 * @param os The output stream to write to.
 * @param loc The Location object to be printed.
 * @return The output stream after writing the Location object.
 */
std::ostream& operator<<(std::ostream& os, const Location& loc)
{
	os << "~~~~~~~~~~~ LOCATION ~~~~~~~~~~~~\n";
	os << "Location Path: " << loc.getPath() << "\n";
	os << "Location Root: " << loc.getRoot() << "\n";
	os << "Location Index: " << loc.getIndex() << "\n";
	loc.ASetting::print(os);
	return (os);
}

/**
 * Overloaded stream insertion operator to print the details of a Server object.
 *
 * @param os The output stream to write the server details to.
 * @param server The Server object whose details are to be printed.
 * @return The output stream after writing the server details.
 */
std::ostream& operator<<(std::ostream& os, const Server& server)
{
	os << "~~~~~~~~~~~~~ SERVER ~~~~~~~~~~~~\n";
	os << "Server Host: " << server.getHost() << "\n";
	os << "Server Port: " << server.getPort() << "\n";
	os << "Server FD: " << server.getFd() << "\n";
	os << "Server name: " << server.getServerName() << "\n";
	server.ASetting::print(os);
	// server.print(os);
	// Location loc = server.findLocation("/");
	// os << loc;
	// os << server.findLocation("/");
	// server.printLocations(os);
	std::map<std::string, Location> loc = server.getLocations();
	for (std::map<std::string, Location>::const_iterator it = loc.begin(); it != loc.end(); ++it)
		os << it->second;
	return (os);
}

/**
 * Overloaded stream insertion operator to print the details of a Config object.
 *
 * @param os The output stream to write the configuration details to.
 * @param config The Config object whose details are to be printed.
 * @return The output stream after writing the configuration details.
 */
std::ostream& operator<<(std::ostream& os, const Config& config)
{
	std::map<std::string, Server> servers = config.getServersMap();
	for (std::map<std::string, Server>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
		os << "==================== " << it->first << " =====================" << "\n";
		os << it->second;
	}
	return (os);
}


/**
 * @brief Trims leading whitespace characters from a given string.
 * This function removes all leading whitespace characters from the input string
 * and returns the resulting string.
 * @param str The input string from which leading whitespace will be removed.
 * @return A new string with leading whitespace removed.
 */

std::string trimLeadingWhitespace(const std::string& str)
{
	std::string result = str;
	std::string::iterator it = result.begin();

	// Iterate through the string until we find a non-whitespace character
	while (it != result.end() && std::isspace(static_cast<unsigned char>(*it))) {
		++it;
	}

	// Erase the leading whitespace characters
	result.erase(result.begin(), it);

	return (result);
}
/**
 * @brief Checks if a string starts with a given prefix.
 * This function checks if the input string starts with the specified prefix.
 * It returns true if the string starts with the prefix, and false otherwise.
 * @param str The input string to be checked.
 * @param prefix The prefix to check for at the beginning of the input string.
 * @return true if the input string starts with the prefix, false otherwise.
 */
bool startsWith(const std::string& str, const std::string& prefix)
{
	if (prefix.size() > str.size())
		return (false);
	return (std::equal(prefix.begin(), prefix.end(), str.begin()));
}
