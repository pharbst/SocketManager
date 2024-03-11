/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerSEPOLL.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 11:22:31 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/11 18:58:52 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

#if defined(__LINUX__) || defined(__linux__)
int					socketManager::_epollfd;

void		socketManager::socketEpoll(InterfaceFunction interfaceFunction) {
	const int	MAX_EVENTS = 10;
	struct epoll_event ready[MAX_EVENTS];
	initEpoll();
	int errorCounter = 0;
	while (true) {
		int numEvents = epoll_wait(_epollfd, ready, MAX_EVENTS, 1000);
		if (numEvents == -1) {
			std::cout << "socketManager::socketEpoll:	Error in epoll_wait" << std::endl;
			errorCounter++;
			if (errorCounter > 0XFFFF)
				throw std::runtime_error("socketManager::socketEpoll:	Critical error: too many errors in epoll_wait");
		}
		for (int i = 0; i < numEvents; ++i) {
			int fd = ready[i].data.fd;
			struct sockData data = SOCKET;
			if (ready[i].events & EPOLLIN)
				data.READ = true;
			else
				data.READ = false;
			if (ready[i].events & EPOLLOUT)
				data.WRITE = true;
			else
				data.WRITE = false;
			if (SERVERSOCKET || SSLACCEPTEVENT) {
				try {
					socketAccept(fd);
					printSocketMap();
				}
				catch (std::exception &e) {
					PRINT_ERROR;
				}
			}
			else if (!SSLSOCKET || SSLESTAB) {
				interfaceFunction(fd, data);
			}
		}
		checkTimeouts();
	}
}

void		socketManager::initEpoll() {
	_epollfd = epoll_create1(0);
	if (_epollfd == -1)
		throw std::runtime_error("socketManager::initEpoll:	Critical epoll error");
	struct epoll_event interest;
	for (std::map<int, sockData>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		interest.events = EPOLLIN | EPOLLOUT;
		interest.data.fd = pair->first;
		for (int i = 0; i < 10; i++) {
			if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, pair->first, &interest) != -1)
				break ;
			if (i == 9) {
				std::cout << "socketManager::initEpoll:	unable to add " << pair->first << " to the epoll instance deleting socket and continue" << std::endl;
				_sockets.erase(pair->first);
			}
		}
	}
}

void		socketManager::epollAdd(int newClient, struct sockData data) {
	struct epoll_event newClientEvent;
	newClientEvent.events = EPOLLIN | EPOLLOUT;
	newClientEvent.data.fd = newClient;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, newClient, &newClientEvent) == -1) {
		if (_sockets[newClient].info.ssl) {
			SSL_shutdown((SSL*)_sockets[newClient].info.sslData.Context);
			SSL_free((SSL*)_sockets[newClient].info.sslData.Context);
		}
		close(newClient);
		throw std::runtime_error("socketManager::epollAdd:	Error adding file descriptor to epoll");
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
	std::cout << "socketAccept called" << std::endl;
	if (!SERVERSOCKET)
		SSLAccept(fd);
	else {
		int newClient = accept(fd, NULL, NULL);
		if (newClient == -1)
			throw std::runtime_error("socketManager::socketAccept:	accept failed");
		struct sockData	data;
		data = SOCKET;
		data.parentSocket = _sockets.find(fd);
		if (_keepAlive > 0)
			data.info.lastActivity = getCurrentTime();
		else
			data.info.lastActivity = 0;
		if (SSLSOCKET) {
			data.info.sslData.Context = NULL;
			data.info.sslData.Context = (void*)SSL_new((SSL_CTX*)SOCKET.info.sslData.Context);
			if (!data.info.sslData.Context) {
				close(newClient);
				throw std::runtime_error("socketManager::socketAccept:	SSL_new failed");
			}
			SSL_set_fd((SSL*)data.info.sslData.Context, newClient);
		}
		ADDSOCKET(newClient, data);
		if (SSLSOCKET)
			SSLAccept(newClient);
	}
}
