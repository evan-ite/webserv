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

int createResponse(const std::string &httpRequest, const ConfigData &confData)
{
    Request request(httpRequest);

    std::string location = request._location;
    std::string path = confData.locations.at(location).path;
    std::string index = confData.locations.at(location).index;
    
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

    std::cout << "------------- RESPONSE ---------------\n";
    std::cout << response.makeResponse();
    return 0;
}
