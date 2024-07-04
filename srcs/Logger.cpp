#include "../includes/server.hpp"

loglevel_e loglevel = LOGLEVEL;

Logger::Logger(loglevel_e _loglevel)
{
	switch (_loglevel)
	{
		case logERROR:
			_buffer << ANSI_RED;
			break;
		case logWARNING:
			_buffer << ANSI_YELLOW;
			break;
		case logINFO:
			_buffer << ANSI_GREEN;
			break;
		case logDEBUG:
			_buffer << ANSI_BLUE;
			break;
		default:
			break;
	}
	_buffer << _loglevel << " : " << ANSI_RESET;
}

Logger::~Logger()
{
	this->_buffer << std::endl;
	std::cerr << this->_buffer.str();
}
