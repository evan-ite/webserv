#ifndef LOGGER_HPP
#define LOGGER_HPP
# include "settings.hpp"

enum loglevel_e
{
	logERROR,
	logWARNING,
	logINFO,
	logDEBUG
};

class Logger
{
public:
	Logger(loglevel_e _loglevel = logERROR);
	~Logger();
	template <typename T>
	Logger& operator<<(T const& value)
	{
		this->_buffer << value;
		return (*this);
	}
private:
	std::ostringstream _buffer;
};

extern loglevel_e loglevel;
const std::string ANSI_RED = "\033[1;31m";
const std::string ANSI_YELLOW = "\033[1;33m";
const std::string ANSI_GREEN = "\033[1;32m";
const std::string ANSI_BLUE = "\033[1;34m";
const std::string ANSI_RESET = "\033[0m";

# define log(level) if (level > loglevel) ; else Logger(level)

#endif
