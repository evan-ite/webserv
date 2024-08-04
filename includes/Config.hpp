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
		// getter
		Server getServer(std::string server_IP) const; // do we need this?
		std::map<std::string, Server> getServersMap(void) const;

		std::map<std::string, Server> _Servers; //make private after debugging
	private:
		Server _tempServer;
		bool locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value);
		void readServer(const std::string &filename);
		void countBraces(std::string line, int *braceCount);
		std::vector<std::string> getPortHost(const std::string& server, const std::string& keyword);
		void parseMultipleServers(std::string server);
		void loadFallback(const std::string &filename);
		void loadServerStruct(const std::string &configString);
		void parse(ASetting* ptr, std::string key, std::string value, std::string line);
		void printServers(void) const;
};

#endif
