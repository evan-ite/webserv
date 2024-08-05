#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "settings.hpp"

class Request
{
	public:
		Request();
		Request(std::string httpRequest);
		Request(const Request &copy);
		~Request();
		Request & operator=(const Request &assign);
		class invalidMethod : public std::exception
		{
		public:
			const char* what() const throw()
			{
				return ("Invalid HTTP method.");
			}
		};

		std::string 	getPath();
		void		 	setPath(std::string &location);
		std::string		getContentType();
		HttpMethod		getMethod();
		int				getContentLen();
		std::string		getBody();
		std::string		getConnection();
		void			setConnection(std::string connection);
		std::string		getsessionId();
		void			resetSessionId();
		std::vector<std::pair<std::string, std::string> >	getFileData();


	private:
		void parse(std::string httpRequest);
		void printFileData();
		std::string findBoundary(const std::string& httpRequest);
		void parsePart(const std::string& part);
		void parseMultipart(const std::string& httpRequest);
		std::string makeName();

		std::string											_path;
		HttpMethod											_method;
		std::string											_body;
		std::string											_contentType;
		int													_contentLength;
		std::string											_host;
		std::string											_userAgent;
		std::string											_connection;
		std::string											_transferEncoding;
		std::string											_sessionId;
		std::vector<std::pair<std::string, std::string> >	_fileData;
};

#endif
