# TODO List

## Classes
-	Server class (config)
-	Request class
-	Response Class
-	Webserv Class

## Done
- [x] loop over map and run servers (in main)
- [X] Thread for listening asynchronous i/o
- [x] Parse HTTP Request into object
- [x] Process request and store in response object
	- [x] check location from request object
	- [x] find path and index in config object
	- [x] figure out content type of body (MIME type) https://www.rfc-editor.org/rfc/rfc1341
	- [x] put info in response object
- [x] Convert response object into HTTP Response
- [x] Print logging
- [x] Handle signals


## Parsing
- [x] Parsing config file into server object -- > Urbano
	- [x] config file into server stack
	- [x] validate servers (valid keys)
	- [x] add default values for empty and invalid keys
	- [x] store servers in map (string 127.0.0.1:8080 - ptr to config object) (on Jans branch in main)
	- [x] Directory Listing on/off (needs item in conf class)

## Start server(s)
- [x] implement constructor that takes map
- [x] set up epoll instance

## Requests
- [x] Post requests
- [x] Delete requests
- [x] Create Request object outside of Response object and parse request object to constructor of response
- [x] Allow specific methods according to location
- [x] Limit client body size for post requests
- [X] Remove double dots from url
- [X] Remove hardcode upload directory name
- [x] Remove the hardcoded file path in htmltemplate fot the delete.

## Response
- [x] Create Request object outside of Response object and parse request object to constructor of response
- [x] Base error pages on config instead of hard code
- [x] Map / function to match error code to reason
- [x] Base connection on request instead of hard code
- [x] Actually do something if connection is "close"

## CGI
- [x] fork CGI (needs item in conf class)
- [x] Second CGI connected to 42 API
- [x] Revoked and renewed API creds
- [x] create env file
- [x] nice throw errors
- [x] change language simple cgi
- [x] put install dotenv lib in readme
- [x] cookies in cgi
- [x] cgi takes executable

## Create some structure
- [x] Divide long class cpp files in seperate files
- [x] Organize content directory
- [x] Good readme with structure explanation of code and explain api stuff
- [x] What is default and what should be in content dir?
- [x] Fallback and defaulr confusing names

## Directory listing
- [x] Directory Listing (needs item in conf class)
	- [x] Template for auto indexing
	- [x] function to fill template
- [x] delete button next to files in dir listing


## Extra
- [x] Implement time out limits set through header (or conf file?)
- [x] Create beautiful html <333
- [ ] Chunked requests
- [x] Siege
- [x] http redirects
- [ ] Check forbidden functions
- [x] Cookiesssss
- [x] Create python script to automate API key
- [x] remove server_name
