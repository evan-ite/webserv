#!/usr/bin/env python3

import os
import cgi
import cgitb
import requests

cgitb.enable()  # Enables CGI error reporting

# Function to fetch campus data from API
def fetch_campus_id():
    campus_url = "https://api.intra.42.fr/v2/campus"  # API endpoint to fetch campus data
    headers = {
        "Authorization": "Bearer b1e16febec7f666a6c618823b5e6045e3b8c68fffdd2c7c234d697635f224d7e",  # Replace with your actual access token
        "Content-Type": "application/json"
    }
    
    try:
        response = requests.get(campus_url, headers=headers)
        response.raise_for_status()  # Raise an exception for HTTP errors
        campus_data = response.json()

        # Find the campus ID for "Berlin"
        berlin_campus_id = None
        for campus in campus_data:
            if campus["name"] == "Berlin":
                berlin_campus_id = campus["id"]
                break
        print("campus id = " + (str)berlin_campus_id)
        if berlin_campus_id:
            # Step 3: Construct URL for fetching users associated with Berlin campus
            users_url = f"https://api.intra.42.fr/v2/campus/{berlin_campus_id}/users"
            
            # Step 4: Make a GET request to fetch users data
            users_response = requests.get(users_url)

            if users_response.status_code == 200:
                return users_response.json()
    except requests.exceptions.RequestException as e:
        print(f"Error fetching campus data: {e}")
        return None

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
print("<h1>Your personalized page <3</h1>")

# Fetch and display campus data from API
user_data = fetch_campus_id()
print(user_data)
# if user_data:
#     print("<h2>Campus Data from API:</h2>")
#     print("<ul>")
#     for campus in user_data:
#         print(f"<li>{campus['name']}</li>")
#     print("</ul>")
# else:
#     print("<p>Failed to fetch campus data from API.</p>")

# Handling GET request parameters
# if os.environ['REQUEST_METHOD'] == 'GET':
#     print("<p>GET Request Parameters:</p>")
#     if form:
#         for key in form.keys():
#             print(f"<p>{key}: {form.getvalue(key)}</p>")
#     else:
#         print("<p>No GET parameters received.</p>")

# # Handling POST request data
# elif os.environ['REQUEST_METHOD'] == 'POST':
#     print("<p>POST Request Data:</p>")
#     if form:
#         for key in form.keys():
#             print(f"<p>{key}: {form.getvalue(key)}</p>")
#     else:
#         print("<p>No POST data received.</p>")

print("</body>")
print("</html>")


