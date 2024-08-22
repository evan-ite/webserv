#include "../includes/settings.hpp"

/**
 * @brief Handles HTTP redirection by setting the appropriate response headers.
 *
 * This function processes a redirection by setting the response status to 301 (Moved Permanently),
 * the reason phrase to "Found", the connection header to "close", and the content length to 0.
 * It also sets the redirection URL (_redir) to the provided redir parameter. If the redir parameter
 * is empty, the function returns false, indicating that no redirection is needed.
 *
 * @param redir The URL to which the request should be redirected.
 * @return true if redirection is handled, false if the redir parameter is empty.
 */
bool Response::handleRedir(std::string redir)
{
	if (redir.empty())
		return (false);
	this->_status = 301;
	this->_reason = "Found";
	this->_connection = "close";
	this->_contentLength = 0;
	this->_redir = redir;
	return (true);
}

/**
 * @brief Handles the execution of a CGI script if the request matches the CGI configuration.
 *
 * This function checks if CGI is enabled for the current location and if a CGI extension
 * is defined. If CGI is not enabled or no CGI extension is defined, it logs an error and
 * returns false. If the request URI matches the CGI extension, it creates a Cgi object
 * and executes the CGI script. If the CGI script is executed, it returns true. Otherwise,
 * it returns false.
 *
 * @param request The HTTP request to be checked and potentially processed by a CGI script.
 * @return true if the CGI script is executed, false otherwise.
 */
bool Response::handleCGI(Request &request)
{
	if (_loc->getCgi() < 1)
		return (false);

	std::string ext = _loc->getCgiExtension();
	if (ext.empty())
	{
		log(logERROR) << "No CGI extension defined";
		return (false);
	}
	if (this->checkCGI(request))
	{
		Cgi cgi(request, _loc);
		cgi.execute(*this);
		return (true);
	}
	else
		return (false);
}

/**
 * @brief Creates files with specified content from a vector of filename-content pairs.
 *
 * This function takes a vector of pairs, where each pair consists of a filename and its
 * corresponding content. It attempts to create and write to each file in the specified
 * directory. If any file operation fails (either opening or writing), it logs an error
 * message and sets a failure flag. After processing all files, if any failure occurred,
 * it throws a ResponseException with a "500" status code. If all files are created
 * successfully, it sets the response status to 201.
 *
 * @param fileData A vector of pairs, where each pair contains a filename and its content.
 * @param file The directory path where the files should be created.
 * @throws ResponseException if any file operation fails.
 */
void Response::createFiles(std::vector<std::pair<std::string, std::string> > &fileData, std::string &file)
{
	bool anyFailure = false;
	for (size_t i = 0; i < fileData.size(); ++i)
	{
		std::string filename = fileData[i].first;
		std::string content = fileData[i].second;
		std::string fullpath = file + filename;
		std::ofstream file(fullpath.c_str(), std::ios::binary);

		if (!file)
		{
			log(logERROR) << "Failed to open file for writing: " << fullpath;
			anyFailure = true;
			continue;
		}
		file.write(content.data(), content.size());
		if (!file.good())
		{
			log(logERROR) << "Failed to write to file: " << fullpath;
			anyFailure = true;
			continue;
		}
		file.close();
	}
	if (anyFailure)
		throw ResponseException("500");
	else
		_status = 201;
}

/**
 * @brief Extracts the file path from the request URI based on the location configuration.
 *
 * This function determines the file path to be used for the request by extracting
 * the relevant portion of the URI and appending it to the root directory specified
 * in the location configuration. If the extracted file path does not contain a file
 * extension and the addIndex flag is true, it appends the index file specified in the
 * location configuration. The function ensures that the resulting file path is relative
 * to the root directory.
 *
 * @param addIndex A boolean flag indicating whether to append the index file if no file extension is found.
 * @return A std::string representing the extracted file path.
 */
std::string Response::extractFilePath(bool addIndex)
{
	std::size_t	endLoc = _uri.find(_loc->getPath()) + _loc->getPath().length();
	std::string filePath;
	std::string	file;

	if (endLoc < _uri.length())
	{
		file = _uri.substr(endLoc);
		if (addIndex && file.find('.') == std::string::npos)
			file += "/" + _loc->getIndex();
	}
	else if (addIndex)
		file = _loc->getIndex();

	if (file[0] && file[0] == '/')
		file = file.substr(1);

	if (file.find(_loc->getRoot()) != std::string::npos)
		filePath = file;
	else
		filePath = this->_loc->getRoot() + "/" + file;

	return (filePath);
}

/**
 * @brief Checks if the MIME type for the given extension is valid.
 *
 * This function iterates over the possible extensions in MIME.txt and checks if the
 * provided extension is valid. If found, it returns the corresponding content type.
 *
 * @param extension The file extension.
 * @return The content type as "type/subtype". Defaults to "text/plain" if no match is found.
 */
std::string checkMime(const std::string &extension)
{
	std::ifstream mime(MIMEFILE);
	if (!mime.is_open())
	{
		log(logERROR) << "Error opening file: " << MIMEFILE;
		return ("");
	}

	std::string line;
	while (std::getline(mime, line))
	{
		std::string::size_type sep = line.find(',');
		std::string temp = line.substr(0, sep);
		if (temp == extension)
		{
			mime.close();
			return (line.substr(sep + 1));
		}
	}
	mime.close();
	return ("text/html");
}

/**
 * @brief Finds the content type for a given filename.
 *
 * This function checks the file extension of the provided filename and returns the
 * corresponding content type. If no match is found, it defaults to "text/plain".
 *
 * @param filename The name of the file.
 * @return The content type as "type/subtype".
 */
std::string findType(const std::string &filename)
{
	std::size_t i = filename.rfind('.');
	if (i == std::string::npos)
		return "text/plain";
	std::string extension = filename.substr(i + 1);

	return checkMime(extension);
}

/**
 * @brief Checks if the request URI matches the CGI extension.
 *
 * This function determines whether the requested URI should be handled by a CGI script
 * based on the extension specified in the location configuration. It checks if the URI
 * ends with the CGI extension or contains the CGI extension followed by a query string.
 *
 * @param request The HTTP request to be checked.
 * @return true if the URI matches the CGI extension, false otherwise.
 */
bool	Response::checkCGI(Request &request)
{
	std::string ext = _loc->getCgiExtension();
	std::size_t	lenExt = ext.length();
	int			finalChars = request.getUri().size() - lenExt;

	if (request.getUri().length() >= lenExt && request.getUri().substr(finalChars) == ext)
		return true;
	else if (request.getUri().find(ext + "?") != std::string::npos)
		return true;
	else
		return false;
}

/**
 * @brief Creates an HTML directory listing using a template file.
 *
 * This function generates an HTML directory listing for the specified directory path
 * by using a template file. It reads the template file, replaces the placeholder "INSERT"
 * with the actual directory listing generated by the loopDir function, and replaces the
 * placeholder "PATH" with the location path. The resulting HTML content is then set as
 * the response body. If the template file cannot be opened, it logs an error and throws
 * a ResponseException with a "500" status code.
 *
 * @param dirPath The path of the directory to be listed.
 * @throws ResponseException if the template file cannot be opened.
 */
void	Response::createDirlisting(std::string dirPath)
{
	std::string templateFile = _loc->getDirlistTemplate();

	std::ifstream templateStream(templateFile.c_str());
	if (!templateStream.is_open())
	{
		log(logERROR) << "Error opening template file: " << templateFile;
		throw ResponseException("500");
	}
	std::stringstream buffer;
	buffer << templateStream.rdbuf();
	templateStream.close();
	std::string templateContent = buffer.str();

	std::string insertList = this->loopDir(dirPath);

	std::size_t insertPos = templateContent.find("INSERT");
	if (insertPos != std::string::npos)
		templateContent.replace(insertPos, 6, insertList);

	std::size_t pathPos = templateContent.find("PATH");

	if (pathPos != std::string::npos)
		templateContent.replace(pathPos, 4, _loc->getPath());

	this->setBody(templateContent);

}

/**
 * @brief Generates an HTML directory listing for the specified directory path.
 *
 * This function recursively traverses the directory specified by dirPath and generates
 * an HTML unordered list (<ul>) containing the names of files and subdirectories. If the
 * DELETE method is allowed for the current location, a delete button is added next to each
 * file and directory name. The function handles errors by logging them and throwing a
 * ResponseException with a "500" status code.
 *
 * @param dirPath The path of the directory to be listed.
 * @return A std::string containing the HTML representation of the directory listing.
 * @throws ResponseException if the directory cannot be opened or closed.
 */
std::string	Response::loopDir(std::string dirPath)
{
	std::ostringstream html;

	struct dirent	*entry;
	DIR		*dir = opendir(dirPath.c_str());
	if (dir == NULL)
	{
		log(logERROR) << "Error opening directory: " << dirPath;
		throw ResponseException("500");
	}

	html << "<ul>";
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			html << "<li> " << entry->d_name;
			if (_loc->findAllow(DELETE))
			{
				html << "<button onclick=\"deleteFile('"
				<< entry->d_name
				<< "')\">Delete</button>";
			}
			html << "</li>";
			if (entry->d_type == DT_DIR)
			{
				std::string	subPath = dirPath + "/" + entry->d_name;
				html << this->loopDir(subPath);
			}
		}
	}
	html << "</ul>";

	if (closedir(dir) != 0)
	{
		log(logERROR) << "Error closing directory: " << dirPath;
		throw ResponseException("500");
	}
	return (html.str());
}
