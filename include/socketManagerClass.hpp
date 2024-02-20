/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerClass.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 17:55:53 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/20 18:29:00 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGERCLASS_HPP
# define SOCKETMANAGERCLASS_HPP

# include "socketManager.hpp"
# include "socketManagerStructs.hpp"

class socketManager {
	public:
		static void		start(InterfaceFunction interfaceFunction);
		static void		stop();
		static void		addServerSocket(struct socketParameter* param);
		// // static void		addClientSocket(struct socketParameter* param);	// not implemented and unsure if nessesary
		static void		removeSocket(int fd);
		static void		initSSL();
		static void		destroySSL();
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
		static void							setSocketNonBlocking(int fd);
		static void							
	/************************************************/
	/*         private depending functions          */
	/************************************************/
	#if defined(__LINUX__) || defined(__linux__)
		static void							socketEpoll(InterfaceFunction interfaceFunction);
		static bool							initEpoll();
		static bool							epollAdd(int newClient, int serverSocket);
		static void							epollRemove(int fd);
		static void							epollAccept(int fd);
	#elif defined(__APPLE__)
		static void							socketKqueue(InterfaceFunction interfaceFunction);
		static bool							initKqueue();
		static bool							kqueueAdd(int newClient, int serverSocket);
		static void							kqueueRemove(int fd);
		static void							kqueueAccept(int fd);
	#else
		static void							socketSelect(InterfaceFunction interfaceFunction);
		static bool							initSelect();
		static bool							selectAdd(int newClient, int serverSocket);
		static void							selectRemove(int fd);
		static void							selectAccept(int fd);
	#endif
};

#endif