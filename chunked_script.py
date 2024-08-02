import http.client

conn = http.client.HTTPConnection("localhost", 8480)

# Construct headers for chunked transfer encoding
headers = {
    "Transfer-Encoding": "chunked",
    "Content-Type": "text/plain"
}

# Start the request
conn.putrequest("POST", "/upload")
for header, value in headers.items():
    conn.putheader(header, value)
conn.endheaders()

# Send chunks
chunks = ["0B\r\nHello my de\r\n", "0B\r\nar friends!\r\n", "0\r\n\r\n"]
for chunk in chunks:
    conn.send(chunk.encode())

# Get the response
response = conn.getresponse()
print(response.status, response.reason)
print(response.read().decode())

conn.close()
