#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "settings.hpp"

class Server;

class Config
{
	public:
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);

		Config &operator=(const Config &rhs);

		std::map<std::string, Server> getServersMap(void) const;

	private:
		Config(void);
		std::map<std::string, Server>	_Servers;
		Server							_tempServer;
		Server							_fallBack;

		bool							locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value);
		void							readServer(const std::string &filename , bool isDefault);
		void							countBraces(std::string line, int *braceCount);
		void							parseMultipleServers(std::string server);
		void							loadServerStruct(const std::string &configString);
		void							parse(ASetting& setting, std::string key, std::string value, std::string line);
		void							parseUnique(Location &location, bool isLoc, std::string key, std::string value, std::string line);
		std::vector<std::string>		getPortHost(const std::string& server, const std::string& keyword);
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif
