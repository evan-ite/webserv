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
- [ ] Parsing config file into server object -- > Urbano
	- [ ] config file into server stack
	- [ ] validate servers (valid keys)
	- [ ] add default values for empty and invalid keys
	- [ ] store servers in map (string 127.0.0.1:8080 - ptr to config object) (on Jans branch in main)
	- [ ] Directory Listing on/off (needs item in conf class)


## Handle requests
- [ ] Post requests
- [ ] Delete requests
- [ ] Limit client body size for post requests
- [ ] Chunked requests, what do we do about them!?!


## CGI
- [ ] fork CGI (needs item in conf class)


## Extra
- [ ] Implement time out limits set through header (or conf file?)
- [ ] Create beautiful html <333
- [ ] http redirects
- [ ] Check forbidden functions
- [ ] Directory Listing (needs item in conf class)
	- [ ] Template for auto indexing 
	- [ ] function to fill template

