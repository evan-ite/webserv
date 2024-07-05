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
    std::string extension = filename.substr(i + 1);

    return checkMime(extension);
}

