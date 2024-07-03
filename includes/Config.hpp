/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: evan-ite <evan-ite@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 15:23:46 by ubazzane          #+#    #+#             */
/*   Updated: 2024/07/03 13:42:22 by evan-ite         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <map>

struct LocationConfig {
	std::string path;
	std::string root;
	std::string index;
	std::string error_page;
	std::string cgi;
	std::string client_max_body_size;
	bool allow_uploads; // = false;

	LocationConfig(const std::string& path) : path(path) {}
};

struct ConfigData {
	std::string server_name;
	std::string host;
	int port;
	std::map<std::string, LocationConfig> locations;
};

class Config {
	public:
		Config(void);
		Config(const std::string &filename);
		Config(const Config &src);
		~Config(void);

		Config &operator=(const Config &rhs);

		void parseConfigFile(const std::string &filename);
		ConfigData getConfigData(void) const;

	private:
		ConfigData _configData;
};

std::ostream& operator<<(std::ostream& os, const ConfigData& configData);

#endif
