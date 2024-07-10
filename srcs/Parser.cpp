#include "../includes/server.hpp"

// Constructors
Parser::Parser() {}
Parser::Parser(const Parser &copy)
{
	(void) copy;
}
// Destructor
Parser::~Parser() {}
// Operators
Parser & Parser::operator=(const Parser &assign)
{
	(void) assign;
	return *this;
}
// Functions
void Parser::parse(std::string confPath, std::map<std::string, Config> *confMap)
{
	(void) confPath;
	(void) confMap;
	return ;
}
