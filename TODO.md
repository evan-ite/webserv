# TODO List

## Classes
-	Server class (config)
-	Request class
-	Response Class
-	Webserv Class

- [ ] Parsing config file into server object -- > Urbano
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



