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

## Handle requests
- [x] Post requests
- [x] Delete requests
- [ ] Limit client body size for post requests
- [ ] Chunked requests, what do we do about them!?!
- [x] Create Request object outside of Response object and parse request object to constructor of response
- [x] Allow specific methods according to location
- [ ] Base connection on request instead of hard code
- [x] Base error pages on config instead of hard code
- [x] Map / function to match error code to reason

## CGI
- [x] fork CGI (needs item in conf class)
- [x] Second CGI connected to 42 API
- [ ] prune Elise's API KEY from the commit history :(
- [x] Revoked and renewed API creds
- [x] create env file 



## Extra
- [x] Implement time out limits set through header (or conf file?)
- [ ] Create beautiful html <333
- [ ] http redirects
- [ ] Check forbidden functions
- [x] Directory Listing (needs item in conf class)
	- [x] Template for auto indexing
	- [x] function to fill template
