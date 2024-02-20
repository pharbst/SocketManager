/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerStructs.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 17:54:31 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/20 17:59:18 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGERSTRUCTS_HPP
# define SOCKETMANAGERSTRUCTS_HPP

#include "socketManager.hpp"

struct socketParameter {
	struct sockaddr* 			interfaceAddress;
	bool						ssl;
	std::string					sslCertificate;
	std::string					sslKey;
	std::vector<std::string>	sslCiphers;
};

struct sslData {
	void*		sslContext;
	bool		established;
	bool		sslRead;
	bool		sslWrite;
};

struct socketInfo {
	struct sslData		ssl;
	uint32_t			port;
	bool				read;
	bool				write;
};

struct sockData {
	struct socketData*	parentSocket;
	struct socketInfo	info;
}

#endif
