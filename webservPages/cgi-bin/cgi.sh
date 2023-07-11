#!/bin/bash

echo -e "HTTP/1.1 200 OK\r
Content-Type: text/html\r
content_length: 127\r
\r
<!DOCTYPE html>
<html>
	<head>
		<title> cgi-page </title>
	</head>
	<body>
		<h1>Hello from the cgi-page</h1>
	</body>
</html>"
