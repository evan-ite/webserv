#include "../includes/settings.hpp"

int g_signal = 1;

void signalhandler(int sig)
{
	if (sig == SIGINT)
		g_signal = 0;
}

// Config testConfig()
// {
// 	Server server;
// 	Config conf;
// 	server.setHost("localhost");
// 	server.setPort(8080);
// 	server.setRoot("./default");
// 	server.setClientMaxBodySize(1024);
// 	server.setCgi(false);

// 	Location rootLocation;
// 	rootLocation.setRoot("default");
// 	rootLocation.addAllow("GET");
// 	rootLocation.setIndex("html/index.html");
// 	rootLocation.setAutoindex(false);

// 	server.addLocation(rootLocation);
// 	conf._Servers["localhost:8080"] = server;

// 	return (conf);
// }

int main(int argc, char **argv)
{
	srand(time(NULL));
	signal(SIGINT, signalhandler);
	if (argc > 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return (EXIT_SUCCESS);
	}
	std::string configPath(argv[1] ? argv[1] : DEFAULT_CONF);
	try
	{
		log(logINFO) << "Starting virtual server(s)";
		Config conf(configPath);
		Webserv webserv;
		// std::map<std::string, Server> serversMap = conf.getServersMap();
		std::map<std::string, Server> serversMap = conf._Servers;
		std::map<std::string, Server>::iterator it = serversMap.begin();
		for (; it != serversMap.end(); it++)
		{
			log(logDEBUG) << "--------------------------------";
			log(logDEBUG) << "first: " << it->first;
			for (std::map<std::string, Location>::iterator locVec = it->second._locations.begin();  locVec != it->second._locations.end(); locVec++)
			{
				locVec->second.setServer(&(it->second));
			}
			log(logDEBUG) << "second: \n" << it->second;
			log(logDEBUG) << "--------------------------------";
		}
		return (1);
		webserv.run(conf);
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}
