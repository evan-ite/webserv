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
		std::map<std::string, Server> allServers = conf.getServersMap();
		Webserv webserv(allServers);
		webserv.run();
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}
