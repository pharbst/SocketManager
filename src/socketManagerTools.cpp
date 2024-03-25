/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerTools.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 21:54:30 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/25 15:05:20 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

void	socketManager::setSocketNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("socketManager::setSocketNonBlocking:	fcntl failed");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("socketManager::setSocketNonBlocking:	fcntl failed");
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("socketManager::setSocketNonBlocking:	setsockopt failed");
}

void	socketManager::bindSocket(int fd, struct sockaddr* address) {
	if (address->sa_family == AF_INET) {
		if (bind(fd, address, sizeof(struct sockaddr_in)) == -1) {
			std::stringstream ss;
			ss << extractPort(address);
			throw std::runtime_error("socketManager::bindSocket:	port: " + ss.str() + " " + std::strerror(errno));
		}
	}
	else if (address->sa_family == AF_INET6) {
		if (bind(fd, address, sizeof(struct sockaddr_in6)) == -1) {
			std::stringstream ss;
			ss << extractPort(address);
			throw std::runtime_error("socketManager::bindSocket:	port: " + ss.str() + " " + std::strerror(errno));
		}
	}
	else
		throw std::runtime_error("socketManager::bindSocket: Unsupported address family");
}

void	socketManager::listenSocket(int fd) {
	if (listen(fd, SOMAXCONN) == -1)
		throw std::runtime_error("socketManager::listenSocket:	listen failed");
}

uint32_t	socketManager::extractPort(struct sockaddr* address) {
	if (address->sa_family == AF_INET)
		return (ntohs(((struct sockaddr_in*)address)->sin_port));
	else if (address->sa_family == AF_INET6)
		return (ntohs(((struct sockaddr_in6*)address)->sin6_port));
	else
		throw std::runtime_error("socketManager::extractPort:	unable to extract port");
}

void				socketManager::detectActivity(int fd) {
	if (_keepAlive == 0)
		return ;
	SOCKET.info.lastActivity = getCurrentTime();
}

unsigned long		socketManager::getCurrentTime() {
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	return static_cast<unsigned long>(currentTime.tv_sec) * 1000 + currentTime.tv_nsec / 1000000;
}

void				socketManager::checkTimeouts() {
	static unsigned long lastCheck = 0;
	if (_keepAlive != 0 && getCurrentTime() - lastCheck > _keepAlive / 4) {
		unsigned long currentTime = getCurrentTime();
		for (std::map<int, struct sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (it->second.parentSocket != _sockets.end() && currentTime - it->second.info.lastActivity > _keepAlive) {
				int fd = it->first;
				it--;
				std::cout << "socketManager::checkTimeouts:	timeout (socket " << fd << ")" << std::endl;
				removeSocket(fd);
			}
		}
		lastCheck = getCurrentTime();
	}
	if (getCurrentTime() - _lastPrint > 10000) {
		printSocketMap();
		_lastPrint = getCurrentTime();
	}
}
