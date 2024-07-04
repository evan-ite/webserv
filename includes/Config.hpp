#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "server.hpp"

struct LocationConfig { //rename to Location?
	std::string path;
	std::string root;
	std::string index;
	std::string error_page;
	std::string cgi;
	std::string client_max_body_size;
	// bool allow_uploads = false; // cpp11
	bool allow_uploads;

	LocationConfig(const std::string& path) : path(path) {}
	LocationConfig() : path("") {};
};

struct ConfigData {
	std::string server_name;
	std::string host;
	int port;
	std::map<std::string, LocationConfig> locations;
};

class Config {
	public:
		Config(void);
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);

		Config &operator=(const Config &rhs);

		void parseConfigFile(const std::string &filename);
		ConfigData getConfigData(void) const;

	private:
		ConfigData _configData;
};

std::ostream& operator<<(std::ostream& os, const ConfigData& configData);

#endif
