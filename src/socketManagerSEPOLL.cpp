/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerSEPOLL.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 11:22:31 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/11 20:22:51 by pharbst          ###   ########.fr       */
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
		int fd = pair->first;
		for (int i = 0; i < 10; i++) {
			if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &interest) != -1)
				break ;
			if (i == 9) {
				std::cout << "socketManager::initEpoll:	unable to add " << fd << " to the epoll instance deleting socket and continue" << std::endl;
				if (SSLSOCKET && SERVERSOCKET)
					SSL_CTX_free((SSL_CTX*)SOCKET.info.sslData.Context);
				else if (SSLSOCKET) {
					SSL_shutdown((SSL*)SOCKET.info.sslData.Context);
					SSL_free((SSL*)SOCKET.info.sslData.Context);
				}
				_sockets.erase(fd);
				close(fd);
			}
		}
	}
}

void		socketManager::epollAdd(int newClient, struct sockData data) {
	struct epoll_event newClientEvent;
	newClientEvent.events = EPOLLIN | EPOLLOUT;
	newClientEvent.data.fd = newClient;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, newClient, &newClientEvent) == -1) {
		if (data.info.ssl) {
			SSL_shutdown((SSL*)data.info.sslData.Context);
			SSL_free((SSL*)data.info.sslData.Context);
		}
		close(newClient);
		throw std::runtime_error("socketManager::epollAdd:	Error adding file descriptor to epoll");
	}
	_sockets.insert(std::pair<int, struct sockData>(newClient, data));
}

void		socketManager::epollRemove(int fd) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		std::cout << "socketManager::epollRemove:	Error removing file descriptor from epoll" << std::endl;
}

#elif defined(__APPLE__)

int					socketManager::_kq;
struct kevent		socketManager::_changes[2];
struct kevent		socketManager::_events[2];

void	socketManager::socketKqueue(InterfaceFunction interfaceFunction) {
	initKqueue();
	int errorCounter = 0;
	while (true) {
		int numEvents = kevent(_kq, _changes, 2, _events, 2, NULL);
		if (numEvents == -1) {
			std::cout << "socketManager::socketKqueue:	Error in kevent" << std::endl;
			errorCounter++;
			if (errorCounter > 0XFFFF)
				throw std::runtime_error("socketManager::socketKqueue:	Critical error: too many errors in kevent");
		}
		for (int i = 0; i < numEvents; ++i) {
			int fd = _events[i].ident;
			struct sockData data = SOCKET;
			if (_events[i].filter == EVFILT_READ)
				data.READ = true;
			else
				data.READ = false;
			if (_events[i].filter == EVFILT_WRITE)
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

void	socketManager::initKqueue() {
	_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("socketManager::initKqueue:	Critical kqueue error");
	for (std::map<int, sockData>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		int fd = pair->first;
		EV_SET(&_changes[0], fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		EV_SET(&_changes[1], fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		for (int i = 0; i < 10; i++) {
			if (kevent(_kq, _changes, 2, NULL, 0, NULL) != -1)
				break ;
			if (i == 9) {
				std::cout << "socketManager::initKqueue:	unable to add " << fd << " to the kqueue instance deleting socket and continue" << std::endl;
				if (SSLSOCKET && SERVERSOCKET)
					SSL_CTX_free((SSL_CTX*)SOCKET.info.sslData.Context);
				else if (SSLSOCKET) {
					SSL_shutdown((SSL*)SOCKET.info.sslData.Context);
					SSL_free((SSL*)SOCKET.info.sslData.Context);
				}
				_sockets.erase(fd);
				close(fd);
			}
		}
	}
}

void	socketManager::kqueueAdd(int newClient, struct sockData data) {
	EV_SET(&_changes[0], newClient, EVFILT_READ, EV_ADD, 0, 0, NULL);
	EV_SET(&_changes[1], newClient, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	if (kevent(_kq, _changes, 2, NULL, 0, NULL) == -1) {
		if (data.info.ssl) {
			SSL_shutdown((SSL*)data.info.sslData.Context);
			SSL_free((SSL*)data.info.sslData.Context);
		}
		close(newClient);
		throw std::runtime_error("socketManager::kqueueAdd:	Error adding file descriptor to kqueue");
	}
	_sockets.insert(std::pair<int, struct sockData>(newClient, data));
}

void	socketManager::kqueueRemove(int fd) {
	EV_SET(&_changes[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	EV_SET(&_changes[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	if (kevent(_kq, _changes, 2, NULL, 0, NULL) == -1)
		std::cout << "socketManager::kqueueRemove:	Error removing file descriptor from kqueue" << std::endl;
}

#else

fd_set		socketManager::_interest;
int			socketManager::_maxfd;

void	socketManager::socketSelect(InterfaceFunction interfaceFunction) {
	initSelect();
	int errorCounter = 0;
	while (true) {
		fd_set readfds = _interest;
		fd_set writefds = _interest;
		int numEvents = select(_maxfd + 1, &readfds, &writefds, NULL, NULL);
		if (numEvents == -1) {
			std::cout << "socketManager::socketSelect:	Error in select" << std::endl;
			errorCounter++;
			if (errorCounter > 0XFFFF)
				throw std::runtime_error("socketManager::socketSelect:	Critical error: too many errors in select");
		}
		for (std::map<int, sockData>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			int fd = it->first;
			struct sockData data = it->second;
			if (FD_ISSET(fd, &readfds))
				data.READ = true;
			else
				data.READ = false;
			if (FD_ISSET(fd, &writefds))
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

void	socketManager::initSelect() {
	FD_ZERO(&_interest);
	for (std::map<int, sockData>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		FD_SET(pair->first, &_interest);
		if (pair->first > _maxfd)
			_maxfd = pair->first;
	}
}

void	socketManager::selectAdd(int newClient, struct sockData data) {
	if (newClient >= FD_SETSIZE) {
		if (data.info.ssl) {
			SSL_shutdown((SSL*)data.info.sslData.Context);
			SSL_free((SSL*)data.info.sslData.Context);
		}
		close(newClient);
		throw std::runtime_error("socketManager::selectAdd:	file descriptor too large");
	}
	FD_SET(newClient, &_interest);
	if (newClient > _maxfd)
		_maxfd = newClient;
	_sockets.insert(std::pair<int, struct sockData>(newClient, data));
}

void	socketManager::selectRemove(int fd) {
	FD_CLR(fd, &_interest);
}

#endif

void	socketManager::socketAccept(int fd) {
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
