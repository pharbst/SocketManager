/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:19:38 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/26 14:21:17 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <map>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <limits.h>
# include <cerrno>
# include <csignal>
# include <sstream>
# include <sys/select.h>
# include <vector>
# include <openssl/ssl.h>
# include <openssl/err.h>

# define TCP		SOCK_STREAM
# define UDP		SOCK_DGRAM
# define LOCALHOST	AF_LOCAL
# define IPV4		AF_INET
# define IPV6		AF_INET6
# define IP			0

struct socketParameter {
	struct sockaddr* 							interfaceAddress;
	int											protocol;
	bool										ssl;
	std::string									sslCertificate;
	std::string									sslKey;
	std::vector<std::string>					sslCiphers;
};

struct sslData {
	void*										Context;
	bool										established;
	bool										read;
	bool										write;
};

struct socketInfo {
	bool										ssl;
	struct sslData								sslData;
	uint32_t									port;
	bool										read;
	bool										write;
};

struct sockData {
	std::map<int, struct sockData>::iterator	parentSocket;
	struct socketInfo							info;
};

#if defined(__LINUX__) || defined(__linux__)
# include <sys/epoll.h>
# define SEPOLL socketManager::socketEpoll
# define ADDSOCKET socketManager::epollAdd
# define SEPOLLREMOVE socketManager::epollRemove
#elif defined(__APPLE__)
# include <sys/event.h>
# define SEPOLL socketManager::socketKqueue
# define ADDSOCKET socketManager::kqueueAdd
# define SEPOLLREMOVE socketManager::kqueueRemove
#else
# define SEPOLL socketManager::socketSelect
# define ADDSOCKET socketManager::selectAdd
# define SEPOLLREMOVE socketManager::selectRemove
#endif

typedef void	(*InterfaceFunction)(int sock, struct sockData sockData);

# define SOCKET			_sockets[fd]
# define SERVERSOCKET	(SOCKET.parentSocket == _sockets.end())
# define SSLSOCKET		SOCKET.info.ssl
# define SSLESTAB		SOCKET.info.sslData.established
# define READ			info.read
# define WRITE			info.write

# define SSLACCEPTEVENT		((SOCKET.info.sslData.read && SOCKET.READ) || (SOCKET.info.sslData.write && SOCKET.WRITE))

class socketManager {
	public:
		static void		start(InterfaceFunction interfaceFunction);
		// static void		stop();
		static void		addServerSocket(struct socketParameter &param);
		// // static void		addClientSocket(struct socketParameter* param);	// not implemented and unsure if nessesary
		static void		removeSocket(int fd);
		// static void		initSSL();
		// static void		destroySSL();
		static void		printSocketMap();

	private:
	/************************************************/
	/*              private attributes              */
	/************************************************/
		static bool								_ssl;
		static std::map<int, struct sockData>	_sockets;
	/************************************************/
	/*         private depending attributes         */
	/************************************************/
	#if defined(__LINUX__) || defined(__linux__)
		static int							_epollfd;
	// #elif defined(__APPLE__)
	// 	static int							_kq;
	// 	static struct kevent				_changes[2];
	// 	static struct kevent				_events[2];
	// #else
	// 	static fd_set						_interest;
	// 	static int							_maxfd;
	#endif

	/************************************************/
	/*              private functions               */
	/************************************************/
		static void							initSSL();
		// static void						destroySSL();

		// for add servers socket
		static void							setSocketNonBlocking(int fd);
		static void							bindSocket(int fd, struct sockaddr* interfaceAddress);
		static void							listenSocket(int fd);
		static uint32_t						extractPort(struct sockaddr* interfaceAddress);
		static SSL_CTX*						createSSLContext(struct socketParameter &params);

		// accept functions
		static void							socketAccept(int fd);
		static void							SSLAccept(int fd);

	/************************************************/
	/*         private depending functions          */
	/************************************************/
	#if defined(__LINUX__) || defined(__linux__)
		// routine
		static void							socketEpoll(InterfaceFunction interfaceFunction);
			static void							initEpoll();
			static void							epollAdd(int newClient, struct sockData data);
			static void							epollRemove(int fd);
	// #elif defined(__APPLE__)
	// 	static void							socketKqueue(InterfaceFunction interfaceFunction);
	// 	static bool							initKqueue();
	// 	static bool							kqueueAdd(int newClient, int serverSocket);
	// 	static void							kqueueRemove(int fd);
	// 	static void							kqueueAccept(int fd);
	// #else
	// 	static void							socketSelect(InterfaceFunction interfaceFunction);
	// 	static bool							initSelect();
	// 	static bool							selectAdd(int newClient, int serverSocket);
	// 	static void							selectRemove(int fd);
	// 	static void							selectAccept(int fd);
	#endif
};

#endif
