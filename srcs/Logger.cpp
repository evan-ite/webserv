#include "../includes/settings.hpp"

loglevel_e loglevel = LOGLEVEL;

/**
 * @brief Logger class constructor.
 *
 * @param _loglevel The log level for the logger.
 */
Logger::Logger(loglevel_e _loglevel)
{
	std::time_t now = std::time(0);
	std::tm* localtm = std::localtime(&now);
	char timeBuffer[20];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtm);
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
	_buffer << timeBuffer << " " << _loglevel << " : " << ANSI_RESET;
}

/**
 * @brief Logger class destructor.
 *
 * Prints the log message stored in the buffer and appends a newline character.
 */
Logger::~Logger()
{
	_buffer << std::endl;
	std::cout << _buffer.str();
}
