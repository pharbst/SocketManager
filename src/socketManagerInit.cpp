/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerInit.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 21:54:30 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/25 19:52:28 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

void	socketManager::setSocketNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl");
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("setsockopt");
}

void	socketManager::bindSocket(int fd, struct sockaddr* address) {
	if (bind(fd, address, sizeof(*address)) == -1)
		throw std::runtime_error("binding failed");
}

void	socketManager::listenSocket(int fd) {
	if (listen(fd, SOMAXCONN) == -1)
		throw std::runtime_error("listen");
}

uint32_t	socketManager::extractPort(struct sockaddr* address) {
	if (address->sa_family == AF_INET)
		return (ntohs(((struct sockaddr_in*)address)->sin_port));
	else if (address->sa_family == AF_INET6)
		return (ntohs(((struct sockaddr_in6*)address)->sin6_port));
	else
		throw std::runtime_error("extractPort");
}
