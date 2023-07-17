import os
import sys

if 'REQUEST_METHOD' in os.environ:
    request_method = os.environ['REQUEST_METHOD']
    if request_method == 'POST':
        status_code = 201
        status_message = "Created"
    elif request_method == 'GET':
        status_code = 200
        status_message = "OK"
    else:
        status_code = 400
        status_message = "Bad Request"
else:
    status_code = 400
    status_message = "Bad Request"

content = "Content-type: text/html\r\n\r\n<html><head><title>Python CGI</title></head><body>"

if request_method == 'GET' and 'QUERY_STRING' in os.environ:
    query_string = os.environ['QUERY_STRING']
elif request_method == 'POST' and 'CONTENT_LENGTH' in os.environ:
    content_length = int(os.environ['CONTENT_LENGTH'])
    query_string =  sys.stdin.read(content_length)
if query_string:
	query_pairs = query_string.split('&')
	content += "<h2>Query parameters:</h2>"
	for pair in query_pairs:
		if '=' in pair:
			key, value = pair.split('=')
			content += f"<p>{key}: {value}</p>"
		else:
			content += "<p>Invalid query parameter</p>"
else:
	content += "<h2>No query parameters provided.</h2>"

content += "</body></html>"

print(f"HTTP/1.1 {status_code} {status_message}\r\n{content}")
