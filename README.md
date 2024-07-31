# Webserv

## Overview

**Webserv** is a project that involves building an NGINX-like server in C++98. The project aims to provide an understanding of how web servers operate and the fundamentals of the HTTP protocol.

## Table of Contents

1. [Features](#features)
2. [Installation](#installation)
3. [Usage](#usage)
4. [Configuration](#configuration)
5. [Collaborators](#collaborators)

## Features

- Serve static files (HTML, CSS, JavaScript, images).
- Handle dynamic content using CGI scripts.
- Support for HTTP methods: GET, POST, and DELETE.
- Non-blocking I/O operations with `epoll`.
- Customizable through configuration files.
- Serve content on multiple ports.
- Default error pages and customizable error handling.
- File upload support.

## Installation

1. **Clone the Repository:**

	```sh
	git clone https://github.com/evan-ite/webserv.git
	cd webserv
	```

2. **Install Dependencies:**

	Install C++ dependecies
	```sh
	sudo apt-get update
	sudo apt-get install g++ make build-essential
	```

	Check if python is installed, otherwise install
	```sh
	python3 --version
	sudo apt-get install python3
	```

	Check if python package installer is installed, otherwise install
	```sh
	pip3 --version
	sudo apt-get install python3-pip
	```

	Install python dependencies
	```sh
	pip install requests python-dotenv
	```

3. **42 Network API:**

	Rename temp.env
	```sh
	mv temp.env .env
	```
	To execute some of the CGI script you need acces to the 42 Network API:  [https://api.intra.42.fr/apidoc]. Read the documentation to retrieve your Client ID and Secret, enter both in `.env` in the root of this directory. Now you can retrieve your API Key as follows
	```sh
	./utils/getApiToken.sh
	```
	Copy your key from `utils/api_token.txt` and paste in `.env`.

4. **Build the Project:**

	```sh
	make
	```

## Usage

Run the server with a configuration file:

```sh
./webserv path/to/configuration/file
```

If no configuration file is provided, the server will use a default configuration defined in conf/fallback.conf.

## Configuration

The server's behavior is controlled by a configuration file, inspired by NGINX configuration files. For undefined settings the default settings from fallback.conf will be used. Below is a sample configuration:

```conf
server {
	listen 80;
	server_name localhost;

	location / {
		root content;
		index html/index.html;
		error_page 404 error/404.html;
	}

	location /cgi-bin {
		allow GET POST;
	}

	location /upload {
		root		content;
		allow		GET POST DELETE;
		autoindex	on;
	}

	location /weather {
		return 301 https://kanikeenkortebroekaan.nl/
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
- `return`: Redirect to internal or external pages.


## Collaborators
- Urbano Bazzanella Filho
- Jan Strozyk
- Elise van Iterson
