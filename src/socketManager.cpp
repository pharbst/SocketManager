/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:33:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/20 18:28:09 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

void	socketManager::start(InterfaceFunction interfaceFunction) {
	if (!_ssl)
		initSSL();
	SEPOLL(interfaceFunction);
}

void	socketManager::stop() {
	if (_ssl)
		destroySSL();
	// stop the socketManager
}

void	socketManager::addServerSocket(const struct socketInfo &info) {
	if (!_ssl)
		initSSL();
	// create a server socket
	// bind the socket to the address
	// listen for incoming connections
}

// void	socketManager::addClientSocket(const struct socketInfo &info) {
// 	if (!_ssl)
// 		initSSL();
// 	// create a client socket
// 	// connect to the server
// }

void	socketManager::removeSocket(int fd) {
	
}

void	socketManager::initSSL() {
	_ssl = true;
	// initialize the SSL library
}

void	socketManager::destroySSL() {
	_ssl = false;
	// destroy the SSL library
}

void	socketManager::printSocketMap() {
	// print the socket map
}


