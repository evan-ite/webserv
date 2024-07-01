Sure! Here's a comprehensive README for your HTTP server project:

---

# Webserv

## Overview

**Webserv** is a project that involves building a custom HTTP server in C++98. This server is designed to handle basic HTTP requests and responses, serve static and dynamic content, and be robust and efficient. The project aims to provide an understanding of how web servers operate and the fundamentals of the HTTP protocol.

## Table of Contents

1. [Introduction](#introduction)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Configuration](#configuration)
6. [Testing](#testing)
7. [Collaborators](#collaborators)

## Introduction

The Hypertext Transfer Protocol (HTTP) is a foundational technology for the World Wide Web. It facilitates the communication between clients (such as web browsers) and servers to deliver web pages and other resources. This project involves creating a custom HTTP server that can handle various HTTP methods and serve both static and dynamic content.

## Features

- Serve static files (HTML, CSS, JavaScript, images).
- Handle dynamic content using CGI scripts.
- Support for HTTP methods: GET, POST, and DELETE.
- Non-blocking I/O operations with `poll` (or equivalent).
- Customizable through configuration files.
- Serve content on multiple ports.
- Default error pages and customizable error handling.
- File upload support.

## Installation

1. **Clone the Repository:**

   ```sh
   git clone https://github.com/yourusername/webserv.git
   cd webserv
   ```

2. **Build the Project:**

   ```sh
   make
   ```

## Usage

Run the server with a configuration file:

```sh
./webserv path/to/configuration/file
```

If no configuration file is provided, the server will use a default configuration.

## Configuration

The server's behavior is controlled by a configuration file, inspired by NGINX configuration files. Below is a sample configuration:

```conf
server {
    listen 80;
    server_name localhost;

    location / {
        root /var/www/html;
        index index.html;
        error_page 404 /404.html;
    }

    location /cgi-bin/ {
        cgi /usr/bin/php-cgi;
    }

    location /uploads/ {
        root /var/www/uploads;
        client_max_body_size 10M;
        allow_uploads on;
    }
}
```

### Key Configuration Options

- `listen`: Port number to listen on.
- `server_name`: The hostname of the server.
- `location`: Defines how to handle requests to specific URL paths.
- `root`: The root directory for serving files.
- `index`: The default file to serve for directory requests.
- `error_page`: Custom error page locations.
- `cgi`: Path to CGI executable.
- `client_max_body_size`: Maximum allowed size for client request bodies.
- `allow_uploads`: Enable or disable file uploads.

## Testing

- Use a web browser to access the server and verify it serves static files.
- Use command-line tools like `curl` to test HTTP methods:
  ```sh
  curl -X GET http://localhost:80/
  curl -X POST http://localhost:80/upload -F "file=@/path/to/file"
  ```
  
## Collaborators
- Urbano Bazzanella Filho
- Jan Strozyk
- Elise van Iterson
