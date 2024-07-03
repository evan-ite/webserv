#include "../includes/server.hpp"

int main(void)
{
	log(logINFO) << "INFO message";
	log(logDEBUG) << "DEBUG message";
	log(logERROR) << "ERROR message";
	log(logWARNING) << "WARNING message";
}
