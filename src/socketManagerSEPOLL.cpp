/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerSEPOLL.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 11:22:31 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/27 19:39:03 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

#if defined(__LINUX__) || defined(__linux__)
int					socketManager::_epollfd;

void		socketManager::initEpoll() {
	_epollfd = epoll_create1(0);
	if (_epollfd == -1)
		throw std::runtime_error("initEpoll");
	struct epoll_event interest;
	for (std::map<int, sockData>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		interest.events = EPOLLIN | EPOLLOUT;
		interest.data.fd = pair->first;
		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, pair->first, &interest) == -1)
			throw std::runtime_error("initEpoll");
	}
}

void		socketManager::socketEpoll(InterfaceFunction interfaceFunction) {
	const int	MAX_EVENTS = 10;
	struct epoll_event ready[MAX_EVENTS];
	initEpoll();
	while (true) {
		int numEvents = epoll_wait(_epollfd, ready, MAX_EVENTS, -1);
		if (numEvents == -1)
			throw std::runtime_error("critical epoll error");
		for (int i = 0; i < numEvents; ++i) {
			int fd = ready[i].data.fd;

			struct sockData data = SOCKET; {
				if (ready[i].events & EPOLLIN)
					data.READ = true;
				else
					data.READ = false;
				if (ready[i].events & EPOLLOUT)
					data.WRITE = true;
				else
					data.WRITE = false;
			}

			if (SERVERSOCKET || SSLACCEPTEVENT)
				socketAccept(fd);
			else if (!SSLSOCKET || SSLESTAB) {
				interfaceFunction(fd, data);
			}
		}
	}
}

void		socketManager::epollAdd(int newClient, struct sockData data) {
	std::cout << "adding socket" << std::endl;
	struct epoll_event newClientEvent;
	newClientEvent.events = EPOLLIN | EPOLLOUT;
	newClientEvent.data.fd = newClient;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, newClient, &newClientEvent) == -1) {
		close(newClient);
		return ;
	}
	_sockets.insert(std::pair<int, struct sockData>(newClient, data));
}

void		socketManager::epollRemove(int fd) {
	if (SERVERSOCKET) {
		std::map<int, struct sockData>::iterator serverSocket = _sockets.find(fd);
		for (std::map<int, struct sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (it->second.parentSocket == serverSocket) {
				epoll_ctl(_epollfd, EPOLL_CTL_DEL, it->first, NULL);
				_sockets.erase(it->first);
			}
		}
		epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL);
		_sockets.erase(fd);
	}
	else {
		epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL);
		_sockets.erase(fd);
	}
}

#elif defined(__APPLE__)
#else
#endif

void	socketManager::socketAccept(int fd) {
	if (!SERVERSOCKET && SSLSOCKET)
		SSLAccept(fd);
	else if (SERVERSOCKET) {
		int newClient = accept(fd, NULL, NULL);
		std::cout << "new client accepted" << std::endl;
		if (newClient == -1)
			std::cout << "error accepting a client" << std::endl;
		else {
			struct sockData	data; {
				data.parentSocket = _sockets.find(fd);
				data.info.port = SOCKET.info.port;
				data.info.read = false;
				data.info.write = false;
				if (SSLSOCKET) {
					data.info.ssl = true;
					data.info.read = false;
					data.info.write = false;
					data.info.sslData.Context = SSL_new((SSL_CTX*)SOCKET.info.sslData.Context);
					if (!data.info.sslData.Context) {
						close(newClient);
						return ;
					}
					SSL_set_fd((SSL*)data.info.sslData.Context, newClient);
					SSLAccept(newClient);
				}
			}
			ADDSOCKET(newClient, data);
			printSocketMap();
		}
	}
}
