#include "../includes/server.hpp"

/* Iterates over the possible extensions in MIME.txt and
checks if the argument extension is valid. If the extension
is found the corresponding content type is returned as 
"type/subtype". If no match is found an empty string wil be returned. */
std::string checkMime(const std::string &extension)
{
    std::ifstream mime(MIMEFILE);
    if (!mime.is_open()) {
        log(logERROR) << "Error opening file: " << MIMEFILE;
        return "";
    }

    std::string line;
    while (std::getline(mime, line)) {
        std::string::size_type sep = line.find(',');
        std::string temp = line.substr(0, sep);
        if (temp == extension) {
            mime.close();
            return line.substr(sep + 1);
        }
    }
    mime.close();
    return "";
}

/* Takes a filename as argument and checks if the extension
is valid, if so the content type will be returned in the form
"type/subtype". If no match is found an empty string wil be returned.*/
std::string findType(const std::string &filename)
{
    std::size_t i = filename.rfind('.');
    if (i == std::string::npos)
        return "text/plain";
    std::string extension = filename.substr(i);

    return checkMime(extension);
}

int createResponse(std::string httpRequest)
{
	Config server("default.conf");

    // std::string httpRequest = "GET /index.html HTTP/1.1\r\n"
    //                         "Host: localhost:80\r\n"
    //                         "User-Agent: Chrome/91.0.4472.124\r\n"
    //                         "Accept: */*\r\n"
    //                         "Accept-Encoding: gzip, deflate, br\r\n"
    //                         "Connection: keep-alive\r\n";

    Request request(httpRequest);

    std::string location = request._location;
    std::string path = server.getConfigData().locations[location].path;
    std::string index = server.getConfigData().locations[location].index;
    
    int         status = 200;
    std::string body = readFileToString(path + index);
    std::string reason = "ok";
    std::string type = findType(index);
    std::string connection = "keep-alive";

    if (body == "" || type == "") {
        status = 404;
        reason = "not found";
        connection = "close";
        type = "";
        body = "";
    }

    Response response(status, reason, type, connection, body);

    std::cout << response.makeResponse();
    return 0;
}
