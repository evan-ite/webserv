#include "../includes/settings.hpp"

loglevel_e loglevel = LOGLEVEL;

/**
 * @brief Constructs a new Logger object with the specified log level.
 *
 * This constructor initializes the Logger object by setting the log level and formatting
 * the log message with a timestamp and appropriate color based on the log level.
 * The timestamp is formatted as "YYYY-MM-DD HH:MM:SS".
 *
 * @param _loglevel The log level for the logger (e.g., logERROR, logWARNING, logINFO, logDEBUG).
 */
Logger::Logger(loglevel_e _loglevel)
{
	std::time_t now = std::time(0);
	std::tm* localtm = std::localtime(&now);
	char timeBuffer[20];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtm);
	switch (_loglevel)
	{
		case logDEBUG:
			_buffer << ANSI_BLUE;
			break;
		case logINFO:
			_buffer << ANSI_GREEN;
			break;
		case logWARNING:
			_buffer << ANSI_YELLOW;
			break;
		case logERROR:
			_buffer << ANSI_RED;
			break;
		default:
			break;
	}
	_buffer << timeBuffer << " " << _loglevel << " : " << ANSI_RESET;
}

Logger::~Logger()
{
	this->_buffer << std::endl;
	std::cerr << this->_buffer.str();
}
