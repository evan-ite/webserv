import os
import requests
from dotenv import load_dotenv, set_key

def get_api_key():
	load_dotenv()
	client_id = os.getenv('UID')
	client_secret = os.getenv('SECRET')

	if not client_id or not client_secret:
		raise ValueError("UID and Secret must be set in the .env file")

	url = "https://api.intra.42.fr/oauth/token"
	payload = {
		'grant_type': 'client_credentials',
		'client_id': client_id,
		'client_secret': client_secret
	}

	response = requests.post(url, data=payload)
	if response.status_code == 200:
		api_key = response.json().get('access_token')
		return api_key
	else:
		raise Exception(f"Failed to get API Key. Status code: {response.status_code}\nResponse: {response.text}")

def update_env_file(api_key):
	load_dotenv()
	env_file = '.env'
	set_key(env_file, 'API_KEY', api_key)

if __name__ == "__main__":
	try:
		api_key = get_api_key()
		update_env_file(api_key)
		print(f"API Key has been added to the .env file")
	except Exception as e:
		print(e)
