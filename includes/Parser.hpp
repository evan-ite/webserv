#ifndef PARSER_HPP
# define PARSER_HPP

# include "server.hpp"

class Parser
{
	public:
		Parser();
		Parser(const Parser &copy);
		~Parser();
		Parser & operator=(const Parser &assign);
		static void parse(std::string confPath, std::map<std::string, Config> *confMap);
	private:
		// add parsing and validation functions here!

};

#endif
