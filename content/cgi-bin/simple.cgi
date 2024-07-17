#!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()  # Enables CGI error reporting

print("<!DOCTYPE html>")
print("<html lang='en'>")
print()  # End of headers

form = cgi.FieldStorage()

print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<link rel='stylesheet' href='../../styles.css'>")
print("<title>Simple CGI Script</title>")
print("</head>")
print("<body>")
print("<h1>Simple CGI Script</h1>")

# Handling GET request parameters
if os.environ['REQUEST_METHOD'] == 'GET':
    print("<p>GET Request Parameters:</p>")
    if form:
        for key in form.keys():
            print(f"<p>{key}: {form.getvalue(key)}</p>")
    else:
        print("<p>No GET parameters received.</p>")

# Handling POST request data
elif os.environ['REQUEST_METHOD'] == 'POST':
    print("<p>POST Request Data:</p>")
    if form:
        for key in form.keys():
            print(f"<p>{key}: {form.getvalue(key)}</p>")
    else:
        print("<p>No POST data received.</p>")

print("</body>")
print("</html>")


