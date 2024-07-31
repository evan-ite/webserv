import http.server
import socketserver
import logging

PORT = 8081
LOG_FILE = "requests.log"

class RequestHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.handle_request()

    def do_POST(self):
        self.handle_request()

    def do_PUT(self):
        self.handle_request()

    def do_DELETE(self):
        self.handle_request()


    def handle_request(self):
        response_body = b'Request received and logged.'
        self.log_request(200)
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.send_header("Content-Length", str(len(response_body)))
        self.end_headers()
        self.wfile.write(response_body)

    def log_request(self, code):
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length) if content_length else b''
        with open(LOG_FILE, "a") as log_file:
            log_file.write(self.requestline + "\n")  # Log the request line
            log_file.write(str(self.headers) + "\n")  # Log the headers
            if body:
                log_file.write(body.decode('utf-8') + "\n")  # Log the body if present
            log_file.write("-" * 40 + "\n")

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    handler = RequestHandler
    httpd = socketserver.TCPServer(("", PORT), handler)
    logging.info(f"Starting server on port {PORT}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info("Stopping server")
