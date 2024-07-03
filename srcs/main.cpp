#include "../includes/server.hpp"

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

// std::string findType(const std::string &filename)
// {

// }

int main(void)
{
	// Config server("default.conf");

    // std::string httpRequest = "GET /index.html HTTP/1.1\r\n"
    //                         "Host: localhost:80\r\n"
    //                         "User-Agent: Chrome/91.0.4472.124\r\n"
    //                         "Accept: */*\r\n"
    //                         "Accept-Encoding: gzip, deflate, br\r\n"
    //                         "Connection: keep-alive\r\n";

    // Request request(httpRequest);

    // std::string location = request._location;
    // std::string path = server.getConfigData().locations[location].path;
    // std::string index = server.getConfigData().locations[location].index;
    
    // int         status = 200;
    // std::string body = readFileToString(path + index);
    // std::string reason = "ok";
    // std::string type = findType(index);
    // std::string connection = "keep-alive";

    // if (body == "") {
    //     status = 404;
    //     reason = "not found";
    //     connection = "close";
    //     type = "";
    // }

    // Response response(status, reason, type, connection, body);

    // std::cout << response.makeResponse();
    std::cout << "hello world!\n";
    return 0;
}
