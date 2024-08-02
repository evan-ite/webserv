#ifndef ASETTING_HPP
# define ASETTING_HPP
# include "settings.hpp"

class ASetting
{
	public:
		virtual ~ASetting() = 0;

		// Setters
		void setRoot(const std::string& root);
		void setDirlistTemplate(const std::string& dirlistTemplate);
		void setCgi(bool cgi);
		void setCgiExtension(const std::string& cgi_extension);
		void setCgiBin(const std::string& cgi_bin);
		void setCgiPass(const std::string& cgi_pass);
		void setAllowUploads(bool allow_uploads);
		void setClientMaxBodySize(int client_max_body_size);
		void setAutoindex(bool autoindex);
		void setIndex(const std::string& index);
		void addErrorPage(std::string error_code, std::string error_page);
		void addAllow(std::string method);

		// Getters
		std::string getRoot() const;
		std::vector<std::string> getAllow() const;
		std::map<std::string, std::string> getErrorPages() const;
		std::map<std::string, std::string> getErrorMessages() const;
		std::string getDirlistTemplate() const;
		bool getCgi() const;
		std::string getCgiExtension() const;
		std::string getCgiBin() const;
		std::string getCgiPass() const;
		std::string getIndex() const;
		bool getAutoindex() const;
		bool getAllowUploads() const;
		int getClientMaxBodySize() const;

	protected:
		std::string													root;
		bool														allow[4]; // 0 = get, 1 = post, 2 = delete
		std::string													dirlistTemplate;
		bool														cgi;
		std::string													cgi_extension;
		std::string													cgi_bin;
		std::string													cgi_pass;
		std::string													index;
		bool														autoindex;
		bool														allow_uploads;
		int 														client_max_body_size;
		std::map<std::string, std::pair<std::string, std::string> >	errors;
		virtual void display() const = 0;
};

#endif
