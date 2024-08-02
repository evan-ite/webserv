import argparse
import requests
import concurrent.futures
import time

def chunked_request(url, method='POST', chunk_size=10):
	def generate_chunks(data, chunk_size):
		for i in range(0, len(data), chunk_size):
			yield data[i:i + chunk_size]

	data = "This is a chunked request body." * 10  # Example data
	headers = {'Transfer-Encoding': 'chunked'}

	with requests.Session() as session:
		req = requests.Request(method, url, headers=headers)
		prepped = session.prepare_request(req)

		# Send the request in chunks
		for chunk in generate_chunks(data, chunk_size):
			prepped.body = chunk
			response = session.send(prepped)
			print(f"Chunk sent, response status: {response.status_code}")

def test_requests(server_url):
	try:
		chunked_request(server_url, method='POST')
		print("Chunked POST request sent successfully.")

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
