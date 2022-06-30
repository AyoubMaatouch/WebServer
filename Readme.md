# WebServer
## because reinventing the wheel is an option 
### Features:
- HTTP 1.1 compliant 
- Never block and client can be bounced properly
- Non-blocking I/O operations
- Listen to multiple IPs
- Listen to multiple ports
- Methods: GET, POST, DELETE
- Able to upload files
- Able to serve a fully static website.
- Supports multipale CGIs.
- Custom error pages
- High availablity
- Resilient! Server never dies :slightly_smiling_face:
- `poll()` as the default event monitor
### Usage
``` bash
$ make -C server/
$ ./webserver server/webserver.conf
```

### Setup Configue file
 ```yaml
 server:
	host: 127.0.0.1
	port: 5050
	server_name: example.com
	location:
		allowed_method: get delete post
		path: /
		root: /Users/aymaatou/Desktop/WebServer/webserver/public
		index: index.html index.php ..
		upload: "upload/path"
		auto_index: on || off
		cgi:
			php: "path/to/php-cgi"
			py: "path/to/python"
			perl: "path/to/perl"
		redirection:
			status: redirection_code
			url: 'url'
	error_page:
        status_code: costume_error_page.html
```
