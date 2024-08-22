#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "settings.hpp"

class Response : public AHeader
{
	public:
		Response();
		Response(Request &request);
		Response(const char *errStatus, Location &loc);

		~Response();

		// getters and setters
		void	setReason(std::string reason);

		// funcs
		std::string	getResponse();

		class ResponseException : public std::exception {
			private:
				std::string _erCode;
			public:
				ResponseException(const std::string& erCode) : _erCode(erCode) {}
				~ResponseException() throw() {}

				const char* what() const throw()
				{ return _erCode.c_str();}
		};

	private:
		// vars
		std::string		_reason;
		std::string		_redir;
		std::string		_dirlistTemplate;
		Location 		*_loc;

		//funcs
		void		checkMethod(HttpMethod method, Request &request);
		void		getMethod(Request &request);
		void		postMethod(Request &request);
		void		deleteMethod(Request &request);

		bool		handleRedir(std::string redir);
		bool		handleCGI(Request &request);
		void		createFiles(std::vector<std::pair<std::string, std::string> > &fileData, std::string &file);
		bool		checkCGI(Request &request);
		void		createDirlisting(std::string dirPath);
		std::string	loopDir(std::string dirPath);
		std::string	extractFilePath(bool addIndex);

};


#endif
