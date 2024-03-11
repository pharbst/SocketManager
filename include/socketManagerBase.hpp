/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerBase.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 13:37:27 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/11 21:01:28 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGERBASE_HPP
# define SOCKETMANAGERBASE_HPP

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

struct sockData {
	std::map<int, struct sockData>::iterator	parentSocket;

	struct socketInfo {
		bool										ssl;

		struct sslData {
			void*										Context;
			bool										established;
			bool										read;
			bool										write;
		};

		struct sslData								sslData;
		uint32_t									port;
		bool										read;
		bool										write;
		unsigned long								lastActivity;
	};

	struct socketInfo								info;
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

# define PRINT_ERROR		std::cout << e.what() << std::endl

#endif