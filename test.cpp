#include "socketManager.hpp"
#include "Interface.hpp"

std::string	httpTest(const std::string &request) {
	(void)request;
	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
	return response;
}

int main() {
	{
		struct socketParameter	newSocket;
		struct sockaddr_in		sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = IPV4;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_port = htons(443);
		newSocket.interfaceAddress = (sockaddr*)&sockAddr;
		newSocket.protocol = TCP;
		newSocket.ssl = true;
		newSocket.sslCertificate = "./crt.pem";
		newSocket.sslKey = "./key.pem";
		try {
			socketManager::addServerSocket(newSocket);
			std::cout << "SSL Server Socket added" << std::endl;
			protocolFunction http = &httpTest;
			Interface::addProtocol(443, http);
		}
		catch (const std::exception &e) {
			std::cout << e.what() << std::endl;
			std::cout << "Couldn't add the socket to the socketManager" << std::endl;
		}
	}
	{
		struct socketParameter	newSocket;
		struct sockaddr_in		sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = IPV4;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_port = htons(80);
		newSocket.interfaceAddress = (sockaddr*)&sockAddr;
		newSocket.protocol = TCP;
		newSocket.ssl = false;
		try {
			socketManager::addServerSocket(newSocket);
			std::cout << "Server Socket added" << std::endl;
			protocolFunction http = &httpTest;
			Interface::addProtocol(80, http);
		}
		catch (const std::exception &e) {
			std::cout << e.what() << std::endl;
			std::cout << "Couldn't add the socket to the socketManager" << std::endl;
		}
	}
	{
		struct socketParameter	newSocket;
		struct sockaddr_in		sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = IPV4;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_port = htons(8080);
		newSocket.interfaceAddress = (sockaddr*)&sockAddr;
		newSocket.protocol = TCP;
		newSocket.ssl = false;
		try {
			socketManager::addServerSocket(newSocket);
			std::cout << "Server Socket added" << std::endl;
			protocolFunction http = &httpTest;
			Interface::addProtocol(8080, http);
		}
		catch (const std::exception &e) {
			std::cout << e.what() << std::endl;
			std::cout << "Couldn't add the socket to the socketManager" << std::endl;
		}
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}
