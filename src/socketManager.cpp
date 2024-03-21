/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:33:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/21 14:02:25 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, struct sockData>		socketManager::_sockets;
bool								socketManager::_ssl = false;
unsigned long						socketManager::_keepAlive = 0;

void	socketManager::start(InterfaceFunction interfaceFunction) {
	std::cout << "socketManager starting" << std::endl;
	if (!_ssl)
		initSSL();
	std::stringstream ss;
	ss << _keepAlive << "ms";
	std::cout << "KeepAlive is set to " << (_keepAlive > 0 ? ss.str() : "false") << std::endl;
	printSocketMap();
	if (_sockets.size() == 0)
		throw std::runtime_error("socketManager::start:	no sockets to manage");
	SEPOLL(interfaceFunction);
}

void	socketManager::stop() {
	if (_ssl)
		destroySSL();
	while (_sockets.size() > 0) {
		std::cout << "remove socket: " << _sockets.begin()->first << std::endl;
		removeSocket(_sockets.begin()->first);
	}
}

void	socketManager::addServerSocket(struct socketParameter &params) {
	if (!_ssl)
		initSSL();
	int proto;
	if (!params.protocol)
		proto = TCP;
	else if (params.protocol != TCP && params.protocol != UDP)
		throw std::runtime_error("socketManager::addServerSocket:	invalid protocol");
	else
		proto = params.protocol;
	int fd = socket(params.interfaceAddress->sa_family, proto, IP);
	if (fd == -1)
		throw std::runtime_error("socketManager::addServerSocket:	socket creation failed");
	struct sockData data;
	try {
		setSocketNonBlocking(fd);
		bindSocket(fd, params.interfaceAddress);
		listenSocket(fd);
		data.info.port = extractPort(params.interfaceAddress);
		delete params.interfaceAddress;
	}
	catch (std::exception &e) {
		delete params.interfaceAddress;
		throw e;
	}
	data.parentSocket = _sockets.end();
	data.info.lastActivity = 0;
	data.info.read = false;
	data.info.write = false;
	data.info.sslData.established = false;
	if (params.ssl) {
		data.info.ssl = true;
		data.info.sslData.Context = createSSLContext(params);
	}
	else
		data.info.ssl = false;
	_sockets.insert(std::pair<int, struct sockData>(fd, data));
}

void	socketManager::removeSocket(int fd) {
	if (_sockets.find(fd) == _sockets.end())
		return ;
	if (SSLSOCKET) {
		SSL_shutdown((SSL*)SOCKET.info.sslData.Context);
		SSL_free((SSL*)SOCKET.info.sslData.Context);
	}
	else if (SSLSOCKET && SERVERSOCKET)
		SSL_CTX_free((SSL_CTX*)SOCKET.info.sslData.Context);
	SEPOLLREMOVE(fd);
	_sockets.erase(fd);
	close(fd);
}

# define _SERVER (it->second.parentSocket == _sockets.end())

void	socketManager::printSocketMap() {
	unsigned long currentTime = getCurrentTime();
	std::cout << "╔══════════════════════Socket Map══════════════════════╗" << std::endl;
	std::cout << "║   fd   ║  port  ║ server ║   SSL  ║  esta  ║ timeout ║" << std::endl;
	for (std::map<int, struct sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
		printf("║%8d║%8d║%8d║%8d║%8d║%9ld║\n", it->first, it->second.info.port, _SERVER, it->second.info.ssl, it->second.info.sslData.established ? 1 : 0, (it->second.parentSocket == _sockets.end()) ? 0 : (_keepAlive > 0) ? (currentTime - it->second.info.lastActivity) / 1000 : 0);
	}
	std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
}


