/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 15:38:59 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/20 12:49:48 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

# include "socketManagerBase.hpp"

class socketManager {
	public:
		// function to start the socket manager
		static void		start(InterfaceFunction interfaceFunction);
		// function to add a server socket
		static void		addServerSocket(struct socketParameter &param);
		// function to remove a socket by its fd from the socket manager
		static void		removeSocket(int fd);
		// funciton to print the whole socket map
		static void		printSocketMap();
		// function to tell the socketmanager that a read or write call has been performed on a socket
		static void		detectActivity(int fd);
		// function to stop the socket manager
		static void		stop();
		// function to init the ssl library
		static void		initSSL();
		// function to destroy the ssl library
		static void		destroySSL();

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
		// static void							initSSL();

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
