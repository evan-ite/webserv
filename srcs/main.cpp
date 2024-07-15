#include "../includes/server.hpp"

int g_signal = 1;

void signalhandler(int sig)
{
	if (sig == SIGINT)
		g_signal = 0;
	log(logINFO) << "SIGINT received, shutting down";
}

int main(int argc, char **argv)
{
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
		//conf.printServers(); //for debugging the servers map.
		std::map<std::string, Server> allServers = conf.getServersMap();
		log(logINFO) << "Size " << allServers.size();

		//debug only
		if (allServers.size() == 0)
		{
			allServers["localhost:8480"].host = "localhost";
			allServers["localhost:8480"].port = 8480;
			allServers["localhost:8480"].root = "html";
			allServers["localhost:8480"].server_name = "server1";
			allServers["localhost:8480"].locations["/"].index = "index.html";
			allServers["localhost:8480"].locations["/"].path = "/";
			allServers["localhost:8480"].locations["/"].root = "html";
		}

		Webserv webserv(allServers);
		webserv.run();
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}
