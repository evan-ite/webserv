import argparse
import requests
import concurrent.futures
import time

def test_requests(server_url):
	try:
		response = requests.get(server_url)
		print(f"Valid GET response: {response.status_code}")

		response = requests.post(server_url, json={"key": "value"})
		print(f"Valid POST response: {response.status_code}")

		response = requests.delete(server_url)
		print(f"Valid DELETE response: {response.status_code}")

		response = requests.request("INVALID_METHOD", server_url)
		print(f"Malformed request response: {response.status_code}")

	except requests.RequestException as e:
		print(f"Error: {e}")

def main():
	parser = argparse.ArgumentParser(description="Web server testing script")
	parser.add_argument("server_url", help="URL of the web server (e.g., http://example.com)")
	parser.add_argument("--num-requests", type=int, default=10, help="Number of requests to send")
	parser.add_argument("--sleep-time", type=float, default=0.1, help="Sleep time between requests in seconds")
	args = parser.parse_args()

	with concurrent.futures.ThreadPoolExecutor(max_workers=args.num_requests) as executor:
		futures = []
		for _ in range(args.num_requests):
			futures.append(executor.submit(test_requests, args.server_url))
			time.sleep(args.sleep_time)
		concurrent.futures.wait(futures)

if __name__ == "__main__":
	main()
