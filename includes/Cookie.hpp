#ifndef COOKIE_HPP
# define COOKIE_HPP
# include "settings.hpp"

class Cookie
{
	public:
		Cookie(std::string sessionId);
		Cookie(const Cookie &copy);
		~Cookie();
		Cookie & operator=(const Cookie &assign);
		// Getters / Setters
		const std::string	getSessionId() const;
		bool				getTimeOut();

	private:
		std::string	_sessionId;
		std::time_t	_timeOut;
};

#endif
