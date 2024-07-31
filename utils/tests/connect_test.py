import socket
import sys

def test_connection(host, port, keepalive):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.settimeout(5)

	try:
		sock.connect((host, int(port)))

		if keepalive == 1:
			print("sending keep-alive")
			request = f"GET / HTTP/1.1\r\nHost: {host}\r\nConnection: keep-alive\r\n\r\n"
		else:
			print("sending close")
			request = f"GET / HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n"
		sock.sendall(request.encode())

		response = sock.recv(1024).decode()
		# print("Response received from server")
		# print(response)

		try:
			sock.sendall(b"")
			print("Connection is kept alive.")
		except socket.error:
			print("Connection is closed by the server.")
	except Exception as e:
		print(f"An error occurred: {e}")
	finally:
		sock.close()

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print("Usage: python3 script.py host:port")
		sys.exit(1)

	host, port = sys.argv[1].split(":")
	test_connection(host, port, 1)
	test_connection(host, port, 1)
	test_connection(host, port, 1)
	test_connection(host, port, 1)
	test_connection(host, port, 1)
	test_connection(host, port, 0)
	test_connection(host, port, 0)
	test_connection(host, port, 0)
	test_connection(host, port, 0)
	test_connection(host, port, 0)
