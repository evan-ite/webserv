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
		void			setClientMaxBodySize(int client_max_body_size);
		void			setAutoindex(bool autoindex);
		void			setIndex(const std::string& index);
		void			addErrorPage(std::string error_code, std::string error_page);
		virtual void	addAllow(std::string method);

		// Getters
		std::string							getRoot() const;
		std::string							getDirlistTemplate() const;
		std::string							getCgiExtension() const;
		std::string							getCgiBin() const;
		std::string							getCgiPass() const;
		std::string							getIndex() const;
		int									getCgi() const;
		int									getAutoindex() const;
		int									getClientMaxBodySize() const;
		bool								findAllow(HttpMethod method);
		std::pair<std::string, std::string>	findError(std::string errorCode); // returns pair of reason and filepath
		std::pair<std::string, std::string>	findError(int errorCode);

		// funcs
		void		print(std::ostream& os) const;

	protected:
		Server*														_server;
		std::string													_root;
		bool														_allow[5]; // 0 = get, 1 = post, 2 = delete, 3 = invalid, 4 = set in location - this could be a single int if we use bitwise ops
		std::string													_dirlistTemplate;
		int															_cgi;
		std::string													_cgi_extension;
		std::string													_cgi_bin;
		std::string													_cgi_pass;
		std::string													_index;
		int															_autoindex;
		int															_client_max_body_size;
		std::map<std::string, std::pair<std::string, std::string> >	_errors;
};

#endif
