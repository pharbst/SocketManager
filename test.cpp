#include "socketManager.hpp"

void	interface(int fd, struct sockData data) {
	(void)fd;
	(void)data;
	// std::cout << "Interface function called" << std::endl;
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
		}
		catch (const std::exception &e) {
			std::cout << e.what() << std::endl;
			std::cout << "Couldn't add the socket to the socketManager" << std::endl;
		}
	}
	InterfaceFunction interfaceFunction = &interface;
	socketManager::start(interfaceFunction);
	return 0;
}
