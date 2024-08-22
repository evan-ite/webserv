#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "settings.hpp"

class Request : public AHeader
{
	public:
		Request();
		Request(Location &location);
		~Request();

		// getters & setters
		std::vector<std::pair<std::string, std::string> >	getFileData();
		void												setLocation(Location& location);
		Location&											getLocation();

		// funcs
		bool			appendBuffer(char *buffer, int bytes_read);
		void			isValidRequest();
		std::string		printRequest();
		void			checkLength();


		class RequestException : public std::exception {
		private:
			std::string _erCode;
		public:
			RequestException(const std::string& erCode) : _erCode(erCode) {}
			~RequestException() throw() {}

			const char* what() const throw()
			{ return _erCode.c_str();}
		};

	private:
		void			parseBody();
		bool			isBodyComplete();
		void			unchunkBody();
		void			parseHeader(std::string header);
		void			assignMethodAndUri(std::string method);
		void			parsePart(const std::string& part);
		void			parseMultipart();
		std::string		makeName();

		bool												_foundHeader;
		Location											_location;
		std::string											_boundary;
		std::string											_rawReq;
		std::vector<std::pair<std::string, std::string> >	_fileData;
};

#endif

