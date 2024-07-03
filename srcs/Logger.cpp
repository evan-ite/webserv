#include "../includes/server.hpp"

loglevel_e loglevel = LOGLEVEL;

Logger::Logger(loglevel_e _loglevel)
{
	this->_buffer << _loglevel << " : ";
}

Logger::~Logger()
{
	this->_buffer << std::endl;
	std::cerr << this->_buffer.str();
}
