#ifndef AHEADER_HPP
# define AHEADER_HPP
# include "settings.hpp"

class AHeader
{
	public:
		virtual ~AHeader() = 0;
		AHeader & operator=(const AHeader &assign);

		// getters and setters
		std::string 	getUri() const;
		void		 	setUri(std::string &location);

		int				getStatus() const;
		void			setStatus(int status);

		HttpMethod		getMethod() const;
		void			setMethod(HttpMethod &method);

		std::string		getBody() const;
		void			setBody(std::string &body);

		std::string		getServerName() const;
		void			setServerName(std::string &sname);

		std::string		getContentType() const;
		void			setContentType(std::string contentType);

		int				getContentLen() const;
		void			setContentLen(int &len);

		std::string		getConnection() const;
		void			setConnection(std::string connection);

		std::string		getSessionId() const;
		void			setSessionId(std::string &sessionId);

		std::string		getEncoding() const;
		void			setEncoding(std::string &encoding);


	protected:
		AHeader();
		int													_status;
		std::string											_uri;
		HttpMethod											_method;
		std::string											_body;
		std::string											_serverName;
		std::string											_contentType;
		int													_contentLength;
		std::string											_connection;
		std::string											_sessionId;
		std::string											_transferEncoding;
};

#endif
