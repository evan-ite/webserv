import argparse
import requests

def test_requests(server_url, num_requests=10):
    try:
        for _ in range(num_requests):
            # Valid GET request
            response = requests.get(server_url)
            print(f"Valid GET response: {response.status_code}")

            # Valid POST request (you can customize the payload)
            response = requests.post(server_url, json={"key": "value"})
            print(f"Valid POST response: {response.status_code}")

            # Valid DELETE request
            response = requests.delete(server_url)
            print(f"Valid DELETE response: {response.status_code}")

            # Malformed request (e.g., invalid method)
            response = requests.request("INVALID_METHOD", server_url)
            print(f"Malformed request response: {response.status_code}")

    except requests.RequestException as e:
        print(f"Error: {e}")

def main():
    parser = argparse.ArgumentParser(description="Web server testing script")
    parser.add_argument("server_url", help="URL of the web server (e.g., http://example.com)")
    parser.add_argument("--num-requests", type=int, default=10, help="Number of requests to send")
    args = parser.parse_args()

    test_requests(args.server_url, num_requests=args.num_requests)
if __name__ == "__main__":
    main()
