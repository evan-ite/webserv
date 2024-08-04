#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "settings.hpp"

class Server;

class Config
{
	public:
		Config(void);
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);
		Config &operator=(const Config &rhs);
		Server getServer(std::string server_IP) const;
		// getter
		std::map<std::string, Server> getServersMap(void) const;

		std::map<std::string, Server> _Servers; //make private after debugging
	private:
		Server _tempServer;

		// void parseLocation(Location *currentLocation, std::string key, std::string value, std::string line);
		// void parseServer(std::string key, std::string value, std::string line);
		bool locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value);
		void readServer(const std::string &filename);
		void countBraces(std::string line, int *braceCount);
		// std::vector<std::string> getPorts(std::string server);
		// std::vector<std::string> getHosts(std::string server);
		std::vector<std::string> getPortHost(const std::string& server, const std::string& keyword);
		void parseMultipleServers(std::string server);
		// void parseConfig(const std::string &filename);
		// void removeCharacter(std::string& str, char charToRemove);

		void loadFallback(const std::string &filename);
		void loadServerStruct(const std::string &configString);
		void parse(ASetting* ptr, std::string key, std::string value, std::string line);
		void printServers(void) const;
};

#endif
