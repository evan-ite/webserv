#include "../includes/settings.hpp"

AHeader::AHeader()
{
	_method = INVALID;
	_status = 200;
	_contentLength = 0;
}

AHeader::~AHeader()
{}

AHeader &AHeader::operator=(const AHeader &assign)
{
	if (this != &assign)
	{
		_status = assign._status;
		_uri = assign._uri;
		_method = assign._method;
		_body = assign._body;
		_contentType = assign._contentType;
		_contentLength = assign._contentLength;
		_connection = assign._connection;
		_sessionId = assign._sessionId;
		_transferEncoding = assign._transferEncoding;
	}
	return (*this);
}

// getters and setters
std::string	AHeader::getUri() const
{
	return (_uri);
}
void	AHeader::setUri(std::string &location)
{
	_uri = location;
}

int	AHeader::getStatus() const
{
	return (_status);
}
void	AHeader::setStatus(int status)
{
	_status = status;
}

HttpMethod	AHeader::getMethod() const
{
	return (_method);
}
void	AHeader::setMethod(HttpMethod &method)
{
	_method = method;
}

std::string	AHeader::getBody() const
{
	return (_body);
}
void	AHeader::setBody(std::string &body)
{
	_body = body;
}

std::string	AHeader::getServerName() const
{
	return (_serverName);
}
void	AHeader::setServerName(std::string &sname)
{
	_serverName = sname;
}

std::string	AHeader::getContentType() const
{
	return (_contentType);
}
void	AHeader::setContentType(std::string contentType)
{
	_contentType = contentType;
}

int	AHeader::getContentLen() const
{
	return (_contentLength);
}
void	AHeader::setContentLen(int &len)
{
	_contentLength = len;
}

std::string		AHeader::getConnection() const
{
	return (_connection);
}
void AHeader::setConnection(std::string connection)
{
	_connection = connection;
}

std::string		AHeader::getSessionId() const
{
	return (_sessionId);
}
void AHeader::setSessionId(std::string &sessionId)
{
	_sessionId = sessionId;
}

std::string		AHeader::getEncoding() const
{
	return (_transferEncoding);
}
void AHeader::setEncoding(std::string &encoding)
{
	_transferEncoding = encoding;
}

