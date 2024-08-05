#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "settings.hpp"

struct Location
{
	std::string	path;
	std::string	root;
	std::string	index;
	std::string	redir;
	bool		cgi;
	std::string	cgi_extension;
	std::string	cgi_bin;
	std::string	cgi_pass;
	bool		autoindex;
	bool 		allow_uploads;

	std::map<std::string, std::string>	loc_error_pages;
	std::vector<std::string>			allow;
	Location(const std::string& path) : path(path) {}
	Location() : path("") {};
};



struct ServerSettings
{
	std::string root;
	std::string host;
	int 		port;
	int 		client_max_body_size;
	bool		client_body_in_file_only;
	int 		client_body_buffer_size;
	int 		client_body_timeout;
	bool		cgi;
	std::string	cgi_extension;
	std::string	cgi_bin;
	std::string	cgi_pass;
	std::string	dirlistTemplate;

	std::map<std::string, std::string> error_pages;
	std::map<std::string, std::string> error_messages;
	std::map<std::string, Location> locations;
};

class Config
{
	public:
		Config(void);
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);

		Config &operator=(const Config &rhs);

		void loadServerStruct(const std::string &configString);
		ServerSettings getServer(std::string server_IP) const;
		std::map<std::string, ServerSettings> getServersMap(void) const;
		void printServers(void) const;
		void printFallback(void) const;


	private:
		std::map<std::string, ServerSettings> _Servers;

	private:
		ServerSettings _tempServer;
		ServerSettings _fallBackServer;

		void parseLocation(Location *currentLocation, std::string key, std::string value, std::string line);
		void parseServer(std::string key, std::string value, std::string line);
		bool locationMode(std::string line, bool *parsingLocation, Location *currentLocation, std::string value);
		void readServer(const std::string &filename);
		void countBraces(std::string line, int *braceCount);
		std::vector<std::string> getPorts(std::string server);
		std::vector<std::string> getHosts(std::string server);
		void parseMultipleServers(std::string server);
		void loadFallback(const std::string &filename);
		void parseConfig(const std::string &filename);
		void removeCharacter(std::string& str, char charToRemove);
		void makeStatusMessages(ServerSettings &server);
};

std::ostream& operator<<(std::ostream& os, const Location& location);
std::ostream& operator<<(std::ostream& os, const ServerSettings& server);

#endif
