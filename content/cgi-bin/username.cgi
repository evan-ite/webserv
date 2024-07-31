#!/usr/bin/env python3

import os
import cgi
import cgitb
import requests
import sqlite3
from http.cookies import SimpleCookie
from dotenv import load_dotenv
import uuid

# Load the .env file
load_dotenv()
api_key = os.getenv('API_KEY')

if api_key is None:
	raise ValueError("API_KEY not found in the environment variables")

cgitb.enable()  # Enables CGI error reporting

# Connect to SQLite database (or create it if it doesn't exist)
conn = sqlite3.connect('user_sessions.db')
c = conn.cursor()

# Create table if it doesn't exist
c.execute('''CREATE TABLE IF NOT EXISTS sessions
			 (username TEXT, session_id TEXT)''')
conn.commit()

# Function to fetch user data from API
def fetch_user(username):
	users_url = f"https://api.intra.42.fr/v2/users/{username}"
	headers = {
		"Authorization": f"Bearer {api_key}",
		"Content-Type": "application/json"
	}
	try:
		user_response = requests.get(users_url, headers=headers)
		user_response.raise_for_status()
		user_data = user_response.json()
		return user_data
	except requests.exceptions.RequestException:
		return None

# Function to store session data in SQLite
def store_session(username, session_id):
	c.execute("INSERT INTO sessions (username, session_id) VALUES (?, ?)", (username, session_id))
	conn.commit()

def get_username_by_session(session_id):
	c.execute("SELECT username FROM sessions WHERE session_id = ?", (session_id,))
	row = c.fetchone()
	return row[0] if row else None

# get vars
form = cgi.FieldStorage()
cookie = SimpleCookie(os.environ.get('HTTP_COOKIE'))
session_id = cookie.get('session_id')

if session_id:
	session_id = session_id.value
else:
	session_id = ""
db_username = get_username_by_session(session_id)
input_username = form.getvalue("username")

# Construct html
print("Content-Type: text/html\n")  # Send HTTP header
print("<!DOCTYPE html>")
print("<html lang='en'>")
print()
print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<link rel='stylesheet' href='../../styles.css'>")
print("<title>42 User Check</title>")
print("</head>")
print("<body>")

if db_username == input_username:
	print("<h1>Welcome back <3</h1>")
elif db_username is not None:
	print("<h1>We don't judge stalkers around here</h1>")
else:
	print("<h1>Welcome, new little friend <3</h1>")
	store_session(input_username, session_id)

if form:
	# Fetch and display user data
	user_data = fetch_user(input_username)  # Corrected to use input_username
	if user_data:
		photo = user_data["image"]["link"]
		if photo:
			print('<div class="memebox">')
			print(f'<img class="student" src="{photo}" alt="Your intra photo" width="400">')
			print('</div>')
			print('<div class="memebox">')
			if db_username == input_username:
				print("<p>Look at your beautiful face</p>")
			else:
				print("<p>Look at this person, who is definitely not you, and their beautiful face</p>")
			print('</div>')
		else:
			print("<p>We couldn't find your photo....</p>")
	else:
		print("<p>I don't think this username belongs to a 42 Berlin student...</p>")
else:
	print("<p>No data received</p>")

print("</body>")
print("</html>")
conn.close()
