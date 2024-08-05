#include "../includes/settings.hpp"

bool Response::isValidRequest(Request &request)
{
	if (request.getPath().find("..") != std::string::npos)
	{
		log(logERROR) << "Invalid request: path contains double dots.";
		return (false);
	}
	if (request.getConnection() != "keep-alive" && request.getConnection() != "close")
	{
		if (request.getConnection().empty())
		{
			request.setConnection("keep-alive");
			return (true);
		}
		else
		{
			log(logERROR) << "Invalid request: connection header is invalid:";
			return (false);
		}
	}
	else
		return (true);
}

// Function that returns the correct file path, based on the URI
// and the root and index from the config file. Returnd empty string
// in the case of directory listing
std::string Response::extractFilePath(Request &request)
{
	// Find end of the location path in the URI
	std::size_t	remainder = request.getPath().find(this->_loc.getPath()) + this->_loc.getPath().length();
	std::string	file;

	if (remainder < request.getPath().length()) // if (resuest.getPath != this._loc.getPath())
	{
		// If URI contains a filename extract it
		file = request.getPath().substr(remainder);
		if (file.find('.') == std::string::npos)
			file += "/" + this->_loc.getIndex();
	}
	else // Else use index file
		file = this->_loc.getIndex();

	if (file[0] && file[0] == '/')
		file = file.substr(1);

	std::string filePath;
	if (this->_loc.getAutoindex())
	{
		// directory listing
		filePath = "";
		this->createDirlisting(this->_loc.getPath());
	}
	else
	{
		if (file.find(this->_loc.getRoot()) != std::string::npos)
			filePath = file;
		else
			filePath = this->_loc.getRoot() + "/" + file;
	}
	return (filePath);
}

void Response::checkMethod(HttpMethod method, Request &request)
{

	void (Response::*funcs[3])(Request &) = {&Response::getMethod, &Response::postMethod, &Response::deleteMethod};

	if (this->_loc.findAllow(method))
		(this->*funcs[method])(request);
	else
		throw ResponseException("405");
}

void Response::createFiles(Request &request, int &status)
{
	std::string file = this->_loc.getRoot() + "/" + this->_loc.getPath() + "/";
	std::vector<std::pair<std::string, std::string> > fileData = request.getFileData();

	if (fileData.empty())
	{
		log(logERROR) << "Bad request: no files to create";
		status = 400;
		return ;
	}
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
		status = 500;
	else
		status = 201;
}

/* Iterates over the possible extensions in MIME.txt and
checks if the argument extension is valid. If the extension
is found the corresponding content type is returned as
"type/subtype". If no match is found 'text/plain' will be returned. */
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
	return ("text/plain");
}

/* Takes a filename as argument and checks if the extension
is valid, if so the content type will be returned in the form
"type/subtype". If no match is found an empty string wil be returned.*/
std::string findType(const std::string &filename)
{
	std::size_t i = filename.rfind('.');
	if (i == std::string::npos)
		return "text/plain";
	std::string extension = filename.substr(i + 1);

	return checkMime(extension);
}

bool Response::handleRedir(std::string redir)
{
	if (redir.empty())
		return (false);
	this->_status = 301;
	this->_reason = "Found";
	this->_connection = "close";
	this->_len = 0;
	this->_redir = redir;
	return (true);
}

bool Response::handleCGI(Request &request)
{
	if (this->_loc.getCgi() < 1)
		return (false);
	Cgi cgi(request, this->_loc);
	if (cgi.isTrue())
	{
		cgi.execute(*this);
		return (true);
	}
	return (false);
}
