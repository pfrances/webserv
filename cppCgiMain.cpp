#include <iostream>
#include <unistd.h>

int main(void) {
	std::cout << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: text/html\r\n"
			<< "\r\n"
			<< "<!DOCTYPE html>\n"
			<< "<html>\n"
			<< "	<head>\n"
			<< "		<title> c++ cgi </title>\n"
			<< "	</head>\n"
			<< "	<body>\n"
			<< "		<h1>Hello from the c++ cgi</h1>\n"
			<< "	</body>\n"
			<< "</html>" << std::endl;
	sleep(10);
	return (0);
}
