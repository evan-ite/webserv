# TODO List

## Classes
-	Server class (config)
-	Request class
-	Response Class
-	Webserv Class

- [ ] Parsing config file into server object -- > Urbano
	- [ ] config file into server stack
	- [ ] validate servers (valid keys)
	- [ ] add default values for empty and invalid keys
	- [ ] store servers in map (string 127.0.0.1:8080 - ptr to config object) (on Jans branch in main)
- [ ] loop over map and run servers
- [ ] Thread for listening asynchronous i/o
- [x] Parse HTTP Request into object --> Jan
- [ ] Process request and store in response object --> Elise
	- [ ] check location from request object
	- [ ] find path and index in config object
	- [x] figure out content type of body (MIME type) https://www.rfc-editor.org/rfc/rfc1341
	- [x] put info in response object
- [x] Convert response object into HTTP Response
- [x] Print logging
- [x] Handle signals
- [ ] Directory Listing (needs item in conf class)
- [ ] fork CGI (needs item in conf class)
- [ ] Limit client body size for post requests
- [ ] Chunked requests, what do we do about them!?!
- [ ] Implement time out limits set through header (or conf file?)
- [ ] http redirects

