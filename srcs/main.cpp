#include "../includes/server.hpp"

int g_signal = 1;

void signalhandler(int sig)
{
	if (sig == SIGINT)
		g_signal = 0;
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
		// map<"127.0.0.1:8080", *conf>
		//std::map<std::string, Config>* allConfigsPtr = new std::map<std::string, Config>;
		//Parser::parse(configPath, allConfigsPtr);
		log(logINFO) << "Starting virtual server(s)";
		// Webserv allServer(allConfigsPtr);
		// allServer.run();
		//delete allConfigsPtr;

		Config debug(configPath);
		std::cout << debug << std::endl;
		Webserv debugServ(debug);
		debugServ.run();
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}
