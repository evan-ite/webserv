import socket
import sys

def test_connection(host, port, keepalive):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.settimeout(5)

	try:
		print(f"Connecting to {host}:{port} with keepalive={keepalive}")
		sock.connect((host, int(port)))

		if keepalive == 1:
			print("Sending keep-alive request")
			request = f"GET / HTTP/1.1\r\nHost: {host}\r\nConnection: keep-alive\r\n\r\n"
		else:
			print("Sending close request")
			request = f"GET / HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n"
		sock.sendall(request.encode())

		response = sock.recv(1024).decode()
		if "Connection: close" in response:
			print("Connection is closed by the server.")
		elif "Connection: keep-alive" in response:
			print("Connection is kept alive.")
	except socket.timeout:
		print("Connection timed out.")
	except socket.error as e:
		print(f"Socket error occurred: {e}")
	except Exception as e:
		print(f"An error occurred: {e}")
	finally:
		sock.close()
		print("Socket closed.")

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print("Usage: python3 script.py host:port")
		sys.exit(1)

	host, port = sys.argv[1].split(":")
	for i in range(5):
		test_connection(host, port, 1)
	for i in range(5):
		test_connection(host, port, 0)
