#ifndef LOCATION_HPP
# define LOCATION_HPP
# include "settings.hpp"

class Location : public ASetting
{
	public:
		Location();
		Location(const std::string& path);
		void display() const;
		std::string							getPath() const;
		std::string							getRedir() const;
		void								setPath(std::string p);
		void								setRedir(std::string p);
		bool								findAllow(HttpMethod method);
		std::pair<std::string, std::string>	findError(std::string errorCode);
		std::pair<std::string, std::string>	findError(int errorCode);
	private:
		std::string	_path;
		std::string	_redir;
};

std::ostream& operator<<(std::ostream& os, Location& loc);

#endif
