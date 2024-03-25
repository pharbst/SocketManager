/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:33:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/25 15:08:23 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, struct sockData>		socketManager::_sockets;
bool								socketManager::_ssl = false;
long								socketManager::_lastPrint = 0;
unsigned long						socketManager::_keepAlive = 120000;

void	socketManager::start(InterfaceFunction interfaceFunction) {
	std::cout << FGreen << "socketManager::start:	Webserver is starting..." << NORMAL << std::endl;
	if (!_ssl)
		initSSL();
	std::stringstream ss;
	ss << _keepAlive << "ms";
	std::cout << "KeepAlive is set to " << (_keepAlive > 0 ? ss.str() : "false") << std::endl;
	printSocketMap();
	if (_sockets.size() == 0)
		throw std::runtime_error("socketManager::start:	no sockets to manage");
	_lastPrint = getCurrentTime();
	SEPOLL(interfaceFunction);
}

void	socketManager::stop() {
	if (_ssl)
		destroySSL();
	std::cout << FYellow << "socketManager::stop:	closing connections..." << NORMAL << std::endl;
	int i = 0;
	while (_sockets.size() > 0) {
		removeSocket(_sockets.begin()->first);
		i++;
	}
	std::cout << FGreen << "socketManager::stop:	" << i << " connections closed" << NORMAL << std::endl;
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
		throw std::runtime_error(e.what());
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
	else {
		data.info.ssl = false;
		data.info.sslData.Context = NULL;
		data.info.sslData.established = false;
		data.info.sslData.read = false;
		data.info.sslData.write = false;
	}
	_sockets.insert(std::pair<int, struct sockData>(fd, data));
}

void	socketManager::removeSocket(int fd) {
	if (_sockets.find(fd) == _sockets.end())
		return ;
	if (SSLSOCKET && !SERVERSOCKET) {
		SSL_shutdown((SSL*)SOCKET.info.sslData.Context);
		SSL_free((SSL*)SOCKET.info.sslData.Context);
	}
	else if (SSLSOCKET && SERVERSOCKET)
		SSL_CTX_free((SSL_CTX*)SOCKET.info.sslData.Context);
	SEPOLLREMOVE(fd);
	_sockets.erase(fd);
	close(fd);
}

void	socketManager::printSocketMap() {
	unsigned long currentTime = getCurrentTime();
	std::cout << "╔══════════════════════Socket Map══════════════════════╗" << std::endl;
	std::cout << "║   fd   ║  port  ║ server ║   SSL  ║  esta  ║ timeout ║" << std::endl;
	for (std::map<int, struct sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
		printf("║%8d║%8d║%8s║%8d║%8d║%9ld║\n", it->first, it->second.info.port, _SERVER, it->second.info.ssl, it->second.info.sslData.established ? 1 : 0, (it->second.parentSocket == _sockets.end()) ? 0 : (_keepAlive > 0) ? (currentTime - it->second.info.lastActivity) / 1000 : 0);
	}
	std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
}


