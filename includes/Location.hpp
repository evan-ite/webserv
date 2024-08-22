#ifndef LOCATION_HPP
# define LOCATION_HPP
# include "settings.hpp"

class Server;

class Location : public ASetting
{
	public:
		Location();
		Location(Server &serv);
		Location(const std::string& path);
		Location(const ASetting& other);
		Location& operator=(const ASetting& other);

		// getters and setters
		std::string	getPath() const;
		std::string	getRedir() const;
		std::string getServerName() const;
		void		setServer(Server* server);
		Server*		getServer(); //debugging only remove later
		void		setPath(std::string path);
		void		setRedir(std::string path);

	private:
		std::string	_path;
		std::string	_redir;
};

std::ostream& operator<<(std::ostream& os, const Location& loc);

#endif
