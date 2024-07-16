#!/bin/bash

# Define environment variables for a GET request
REQUEST_METHOD="GET"
QUERY_STRING="name=John&age=30"
SCRIPT_NAME="./content/cgi-bin/simple.py" 
SERVER_PROTOCOL="HTTP/1.1"
SERVER_SOFTWARE="MyServer/1.0"

# Export environment variables
export REQUEST_METHOD
export QUERY_STRING
export SCRIPT_NAME
export SERVER_PROTOCOL
export SERVER_SOFTWARE

# Run the CGI script
python3 ./content/cgi-bin/simple.py
