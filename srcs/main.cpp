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
		// std::map<std::string, Config>* allConfigsPtr = new std::map<std::string, Config>;
		// Parser::parse(configPath, allConfigsPtr); //uses stack internally
		// std::map<std::string, Config> ::iterator it;
		// for (it = (*allConfigsPtr).begin(); it != (*allConfigsPtr).end(); it++)
		// {
		// 	log(logINFO) << "Starting server " << it->first;
		// 	Webserv server(it->second);
		// 	server.run();
		// }
		// delete allConfigsPtr;

		Config debug(configPath);
		Webserv debugServ(debug);
		debugServ.run();
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}

}
