/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:19:38 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/14 21:54:24 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

# include "socketManagerBase.hpp"

class socketManager {
	public:
		static void		start(InterfaceFunction interfaceFunction);
		// static void		stop();
		static void		addServerSocket(struct socketParameter &param);
		static void		removeSocket(int fd);
		// static void		initSSL();
		// static void		destroySSL();
		static void		printSocketMap();
		static void		detectActivity(int fd);

	private:
	/************************************************/
	/*              private attributes              */
	/************************************************/
		static bool								_ssl;
		static std::map<int, struct sockData>	_sockets;
		static unsigned long					_keepAlive;
	/************************************************/
	/*         private depending attributes         */
	/************************************************/
	#if defined(__LINUX__) || defined(__linux__)
		static int							_epollfd;
	#elif defined(__APPLE__)
		static int							_kq;
		static struct kevent				_changes[2];
		static struct kevent				_events[2];
	#else
		static fd_set						_interest;
		static int							_maxfd;
	#endif

	/************************************************/
	/*              private functions               */
	/************************************************/
		static void							initSSL();

		// for add servers socket
		static void							setSocketNonBlocking(int fd);
		static void							bindSocket(int fd, struct sockaddr* interfaceAddress);
		static void							listenSocket(int fd);
		static uint32_t						extractPort(struct sockaddr* interfaceAddress);
		static SSL_CTX*						createSSLContext(struct socketParameter &params);

		// accept functions
		static void							socketAccept(int fd);
		static void							SSLAccept(int fd);

		// routines
		static void							checkTimeouts();
		static unsigned long				getCurrentTime();

	/************************************************/
	/*         private depending functions          */
	/************************************************/
	#if defined(__LINUX__) || defined(__linux__)
		static void							socketEpoll(InterfaceFunction interfaceFunction);
		static void							initEpoll();
		static void							epollAdd(int newClient, struct sockData data);
		static void							epollRemove(int fd);
	#elif defined(__APPLE__)
		static void							socketKqueue(InterfaceFunction interfaceFunction);
		static void							initKqueue();
		static void							kqueueAdd(int newClient, struct sockData data);
		static void							kqueueRemove(int fd);
	#else
		static void							socketSelect(InterfaceFunction interfaceFunction);
		static void							initSelect();
		static void							selectAdd(int newClient, struct sockData data);
		static void							selectRemove(int fd);
	#endif
};

#endif
