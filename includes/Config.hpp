#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "server.hpp"

struct Location { //rename to Location?
	std::string path;
	std::string root;
	std::string index;
	std::string error_page;
	std::string cgi;
	std::string client_max_body_size;
	// bool allow_uploads = false; // cpp11
	bool allow_uploads;

	Location(const std::string& path) : path(path) {}
	Location() : path("") {};
};

struct Server {
	std::string server_name;
	std::string host;
	int port;
	std::map<std::string, Location> locations;
};

class Config {
	public:
		Config(void);
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);

		Config &operator=(const Config &rhs);

		void parseConfigFile(const std::string &filename);
		Server getConfigData(void) const;

	private:
		Server _configData;
};

std::ostream& operator<<(std::ostream& os, const Config& obj);

#endif
