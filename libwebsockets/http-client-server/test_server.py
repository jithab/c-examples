from http.server import BaseHTTPRequestHandler, HTTPServer

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    protocol_version = 'HTTP/1.1'
    def do_GET(self):
        # Print the request headers
        print("Received request headers:")
        for header, value in self.headers.items():
            print(f"{header}: {value}")

        # Send response headers
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

        # Send response body
        response_body = "<G></T>"
        self.wfile.write(response_body.encode('utf-8'))

    def do_POST(self):
        # Same for POST requests, if needed
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)

        print("Received request headers:")
        for header, value in self.headers.items():
            print(f"{header}: {value}")

        print("\nReceived POST data:", post_data.decode('utf-8'))

        # Send response headers
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

        # Send response body
        response_body = "<h></hl>"
        self.wfile.write(response_body.encode('utf-8'))

def run(server_class=HTTPServer, handler_class=SimpleHTTPRequestHandler, port=8080):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting HTTP server on port {port}")
    httpd.serve_forever()

if __name__ == "__main__":
    run()
