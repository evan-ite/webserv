#include "../includes/settings.hpp"

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
	return (0);
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


/*Generates a pseudorandom alphanumeric string of 'length' characters*/
std::string generateRandomString(int length)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string result;
	result.reserve(length);

	for (int i = 0; i < length; ++i)
		result += charset[rand() % (sizeof(charset) - 1)];

	return (result);
}

// Function that removes substr from str and returns the new string.
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
 * @brief Checks if a string ends with a given postfix.
 * This function checks if the input string ends with the specified postfix.
 * It returns true if the string ends with the postfix, and false otherwise.
 * It always returns false if either of the strings is empty.
 * @param str The input string to be checked.
 * @param postfix The postfix to check for.
 * @return true if the input string ends with the postfix, false otherwise.
 */
bool endsWith(const std::string& str, const std::string& postfix)
{
	if (str.empty() || postfix.empty())
		return (false);
	if (postfix.size() > str.size())
		return (false);
	return (std::equal(postfix.rbegin(), postfix.rend(), str.rbegin()));
}
