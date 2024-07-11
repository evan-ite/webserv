#!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()  # Enables CGI error reporting

print("Content-Type: text/html")
print()  # End of headers

form = cgi.FieldStorage()

print("<html>")
print("<head>")
print("<title>Simple CGI Script</title>")
print("</head>")
print("<body>")
print("<h2>Simple CGI Script</h2>")

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
