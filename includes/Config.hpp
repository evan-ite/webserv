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
	std::string root;
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

		void loadServerStruct(const std::string &configString);
		//Server getServer(void) const; // Used to compile while the rest of codebase is incompatible with the Map formatt.
		Server getServer(std::string server_IP) const;
		std::map<std::string, Server> getServersMap(void) const;


	private:
		std::map<std::string, Server> _Servers;
		Server _tempServer;
		Server _fallBackServer;

		void parseLocation(Location *currentLocation, std::string key, std::string value);
		void parseServer(std::string key, std::string value);
		bool locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value);
		void readServer(const std::string &filename);
		void countBraces(std::string line, int *braceCount);
		std::vector<std::string> getPorts(std::string server);
		std::vector<std::string> getHosts(std::string server);
		void parseMultipleServers(std::string server);
		void loadFallback(const std::string &filename);
		void parseConfig(const std::string &filename);
};

//std::ostream& operator<<(std::ostream& os, const Config& obj);

#endif
