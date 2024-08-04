#ifndef ASETTING_HPP
# define ASETTING_HPP
# include "settings.hpp"

class Server;

class ASetting
{
	public:
		ASetting();
		ASetting(const ASetting& other);
		virtual ~ASetting() = 0;
		virtual ASetting& operator=(const ASetting& other);
		// Setters
		void			setRoot(const std::string& root);
		void			setDirlistTemplate(const std::string& dirlistTemplate);
		void			setCgi(bool cgi);
		void			setCgiExtension(const std::string& cgi_extension);
		void			setCgiBin(const std::string& cgi_bin);
		void			setCgiPass(const std::string& cgi_pass);
		void			setAllowUploads(bool allow_uploads);
		void			setClientMaxBodySize(int client_max_body_size);
		void			setAutoindex(bool autoindex);
		void			setIndex(const std::string& index);
		void			addErrorPage(std::string error_code, std::string error_page);
		bool			findAllow(HttpMethod method);
		virtual void	addAllow(std::string method);
		// Getters
		std::string	getRoot() const;
		std::string	getDirlistTemplate() const;
		std::string	getCgiExtension() const;
		std::string	getCgiBin() const;
		std::string	getCgiPass() const;
		std::string	getIndex() const;
		int			getCgi() const;
		int			getAutoindex() const;
		int			getAllowUploads() const;
		int			getClientMaxBodySize() const;
		void		print(std::ostream& os) const;
	protected:
		// vars
		Server*														server;
		std::string													root;
		bool														allow[4]; // 0 = get, 1 = post, 2 = delete, 3 = invalid, 4 = set in location - this could be a single int if we use bitwise ops
		std::string													dirlistTemplate;
		int															cgi;
		std::string													cgi_extension;
		std::string													cgi_bin;
		std::string													cgi_pass;
		std::string													index;
		int															autoindex;
		int															allow_uploads;
		int															client_max_body_size;
		std::map<std::string, std::pair<std::string, std::string> >	errors;
};

std::ostream& operator<<(std::ostream& os, const ASetting& setting);

#endif
