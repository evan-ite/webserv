/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubazzane <ubazzane@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 15:30:39 by ubazzane          #+#    #+#             */
/*   Updated: 2024/07/02 16:39:56 by ubazzane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"

Config::Config(void) {}

Config::Config(const std::string &filename) {
	this->parseConfigFile(filename);
}

Config::Config(const Config &src) {
	*this = src;
}

Config::~Config(void) {}

Config &Config::operator=(const Config &rhs) {
	if (this != &rhs) {
		this->_configData = rhs._configData;
	}
	return *this;
}

void Config::parseConfigFile(const std::string &filename) {
	std::ifstream file(filename.c_str());
	std::string line;
	bool parsingLocation = false;
	LocationConfig currentLocation("/");

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file");

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		std::string value;

		if (line.find("location") != std::string::npos) {
			if (parsingLocation) {
				this->_configData.locations[currentLocation.path] = currentLocation;
				parsingLocation = false;
			}
			else {
				iss >> key >> value; // Assuming 'location /path/' format
				currentLocation = LocationConfig(value);
				parsingLocation = true;
			}
			continue;
		}
		if (!parsingLocation) {
			if (key == "server_name")
				this->_configData.server_name = value;
			else if (key == "host")
				this->_configData.host = value;
			else if (key == "port") {
				std::istringstream iss(value);
				int port;
				if (!(iss >> port))
					throw std::runtime_error("Error: invalid port number");
				this->_configData.port = port;
			}
		}
		else {
			if (key == "root")
				currentLocation.root = value;
			else if (key == "index")
				currentLocation.index = value;
			else if (key == "error_page")
				currentLocation.error_page = value;
			else if (key == "cgi")
				currentLocation.cgi = value;
			else if (key == "client_max_body_size")
				currentLocation.client_max_body_size = value;
			else if (key == "allow_uploads") {
				currentLocation.allow_uploads = (value == "on");
			}
		}
	}
	if (parsingLocation) {
		this->_configData.locations[currentLocation.path] = currentLocation;
	}
	file.close();
}

ConfigData Config::getConfigData(void) const {
	return this->_configData;
}

std::ostream& operator<<(std::ostream& os, const ConfigData& configData) {
	os << "Server Name: " << configData.server_name << std::endl;
	os << "Host: " << configData.host << std::endl;
	os << "Port: " << configData.port << std::endl;
	os << "Locations:" << std::endl;
	for (const auto& location : configData.locations) {
		os << "  Path: " << location.second.path << std::endl;
		os << "    Root: " << location.second.root << std::endl;
		os << "    Index: " << location.second.index << std::endl;
		os << "    Error Page: " << location.second.error_page << std::endl;
		os << "    CGI: " << location.second.cgi << std::endl;
		os << "    Client Max Body Size: " << location.second.client_max_body_size << std::endl;
		os << "    Allow Uploads: " << (location.second.allow_uploads ? "true" : "false") << std::endl;
	}
	return os;
}
