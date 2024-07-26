#!/usr/bin/env python3

import os
import cgi
import cgitb
import requests
from dotenv import load_dotenv

# Load the .env file
load_dotenv()
api_key = os.getenv('API_KEY')

if api_key is None:
    raise ValueError("API_KEY not found in the environment variables")
 
cgitb.enable()  # Enables CGI error reporting

# Function to fetch campus data from API
def fetch_user():
    campus_url = "https://api.intra.42.fr/v2/campus"  # API endpoint to fetch campus data
    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json"
    }
    try:
        # Construct URL for fetching user based on form input
        id = form.getvalue("username")
        users_url = f"https://api.intra.42.fr/v2/users/{id}"
        
        # Make a GET request to fetch users data
        user_response = requests.get(users_url, headers=headers)
        user_response.raise_for_status()
        user_data = user_response.json()

        if user_data:
            return user_data
        
        return None
    
    except requests.exceptions.RequestException as e:
        return None


# Construct html

print("<!DOCTYPE html>")
print("<html lang='en'>")
print()

form = cgi.FieldStorage()

print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<link rel='stylesheet' href='../../styles.css'>")
print("<title>User</title>")
print("</head>")
print("<body>")
print("<h1>Your personalized page <3</h1>")

if form:
    # Fetch and display user data
    user_data = fetch_user()

    if user_data:
        photo = user_data["image"]["link"]
        if photo:
            print('<div class="memebox">')
            print(f'<img class="student" src="{photo}" alt="Your intra photo" width="400">')
            print('</div>')
            print('<div class="memebox">')
            print("<p>Look at your beautiful face</p>")
            print('</div>')
        else:
            print("<p>We couldn't find your photo....</p>")
    else:
            print("<p>I don't think you're a 42 Berlin student...</p>")
else:
    print("<p>No data recieved</p>")
    
print("</body>")
print("</html>")


