/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:33:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/25 23:57:44 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, struct sockData>		socketManager::_sockets;
bool								socketManager::_ssl = false;

void	socketManager::start(InterfaceFunction interfaceFunction) {
	std::cout << "socketManager starting" << std::endl;
	if (!_ssl)
		initSSL();
	printSocketMap();
	SEPOLL(interfaceFunction);
}

// void	socketManager::stop() {
// 	if (_ssl)
// 		destroySSL();
// 	// stop the socketManager
// }

void	socketManager::addServerSocket(struct socketParameter &params) {
	if (!_ssl)
		initSSL();
	// create a server socket
	int fd = socket(params.interfaceAddress->sa_family, params.protocol, IP);
	// set the socket to non-blocking
	setSocketNonBlocking(fd);
	// bind the socket to the address
	bindSocket(fd, params.interfaceAddress);
	// listen for incoming connections
	listenSocket(fd);
	// create the socketData
	struct sockData data; {	
		data.parentSocket = _sockets.end();
		data.info.port = extractPort(params.interfaceAddress);
		data.info.read = false;
		data.info.write = false;
		if (params.ssl) {
			data.info.ssl = true;
			data.info.sslData.Context = createSSLContext(params);
		}
		else
			data.info.ssl = false;
	}
	_sockets.insert(std::pair<int, struct sockData>(fd, data));
}

// void	socketManager::addClientSocket(const struct socketInfo &info) {
// 	if (!_ssl)
// 		initSSL();
// 	// create a client socket
// 	// connect to the server
// }

void	socketManager::removeSocket(int fd) {
	SEPOLLREMOVE(fd);
}

# define _SERVER (it->second.parentSocket == _sockets.end())

void	socketManager::printSocketMap() {
	// print the socket map
	std::cout << "╔══════Socket Map═══════════╗" << std::endl;
	std::cout << "║  fd  ║ port ║server║  SSL ║" << std::endl;
	for (std::map<int, struct sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
		printf("║%6d║%6d║%6d║%6d║\n", it->first, it->second.info.port, _SERVER, it->second.info.ssl);
	}
	std::cout << "╚═══════════════════════════╝" << std::endl;
}


