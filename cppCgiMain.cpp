#include <iostream>
#include <unistd.h>

int main(void) {
	sleep(10);
	std::cout << "HTTP/1.1 200 OK\r" << std::endl;
	std::cout << "Content-Type: text/html\r" << std::endl;
	std::cout << "\r" << std::endl;
	std::cout << "<!DOCTYPE html>" << std::endl;
	std::cout << "<html>" << std::endl;
	std::cout << "	<head>" << std::endl;
	std::cout << "		<title> c++ cgi </title>" << std::endl;
	std::cout << "	</head>" << std::endl;
	std::cout << "	<body>" << std::endl;
	std::cout << "		<h1>Hello from the c++ cgi</h1>" << std::endl;
	std::cout << "	</body>" << std::endl;
	std::cout << "</html>" << std::endl;
	return (0);
}
