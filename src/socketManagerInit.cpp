/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerInit.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 21:54:30 by pharbst           #+#    #+#             */
/*   Updated: 2024/02/20 22:01:47 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

void	socketManager::initSSL() {
	SSL_library_init();
	SSL_load_error_strings();
	_ssl = true;
}

// void	socketManager::destroySSL() {
// 	ERR_free_strings();
// 	EVP_cleanup();
// 	_ssl = false;
// }

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
		throw std::runtime_error("bind");
}

void	socketManager::listenSocket(int fd) {
	if (listen(fd, SOMAXCONN) == -1)
		throw std::runtime_error("listen");
}

SSL_CTX*	socketManager::createSSLContext(struct socketParameter &params) {
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_server_method());
	if (!ctx)
		throw std::runtime_error("SSL_CTX_new");
	if (SSL_CTX_use_certificate_file(ctx, params.sslCertificate.c_str(), SSL_FILETYPE_PEM) <= 0)
		throw std::runtime_error("SSL_CTX_use_certificate_file");
	if (SSL_CTX_use_PrivateKey_file(ctx, params.sslKey.c_str(), SSL_FILETYPE_PEM) <= 0)
		throw std::runtime_error("SSL_CTX_use_PrivateKey_file");
	if (SSL_CTX_check_private_key(ctx) != 1)
		throw std::runtime_error("SSL_CTX_check_private_key");
	if (params.sslCiphers.size() > 0) {
		std::string ciphers;
		for (std::vector<std::string>::iterator it = params.sslCiphers.begin(); it != params.sslCiphers.end(); it++) {
			ciphers += *it;
			if (it + 1 != params.sslCiphers.end())
				ciphers += ":";
		}
		if (SSL_CTX_set_cipher_list(ctx, ciphers.c_str()) != 1)
			throw std::runtime_error("SSL_CTX_set_cipher_list");
	}
	return (ctx);
}

uint32_t	socketManager::extractPort(struct sockaddr* address) {
	if (address->sa_family == AF_INET)
		return (ntohs(((struct sockaddr_in*)address)->sin_port));
	else if (address->sa_family == AF_INET6)
		return (ntohs(((struct sockaddr_in6*)address)->sin6_port));
	else
		throw std::runtime_error("extractPort");
}
