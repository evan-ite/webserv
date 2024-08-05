#include "../includes/settings.hpp"

int g_signal = 1;

void signalhandler(int sig)
{
	if (sig == SIGINT)
		g_signal = 0;
}

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
		Webserv webserv(conf);
		webserv.run();
	}
	catch (std::exception &e)
	{
		log(logERROR) << e.what();
	}
}
