/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:33:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/20 22:13:52 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, struct sockData>		socketManager::_sockets;
bool								socketManager::_ssl = false;

// void	socketManager::start(InterfaceFunction interfaceFunction) {
// 	if (!_ssl)
// 		initSSL();
// 	SEPOLL(interfaceFunction);
// }

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
		data.parentSocket = INVALID_SOCKET_ITERATOR;
		data.info.port = extractPort(params.interfaceAddress);
		data.info.read = false;
		data.info.write = false;
		if (params.ssl)
			data.info.ssl.Context = createSSLContext(params);
	}
	_sockets.insert(std::pair<int, struct sockData>(fd, data));
}

// void	socketManager::addClientSocket(const struct socketInfo &info) {
// 	if (!_ssl)
// 		initSSL();
// 	// create a client socket
// 	// connect to the server
// }

// void	socketManager::removeSocket(int fd) {
	
// }

// void	socketManager::initSSL() {
// 	_ssl = true;
// 	// initialize the SSL library
// }

// void	socketManager::destroySSL() {
// 	_ssl = false;
// 	// destroy the SSL library
// }

// void	socketManager::printSocketMap() {
// 	// print the socket map
// }


