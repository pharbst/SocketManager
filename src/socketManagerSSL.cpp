/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerSSL.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 13:05:01 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/25 12:39:41 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

void	socketManager::initSSL() {
	SSL_library_init();
	SSL_load_error_strings();
	_ssl = true;
}

void	socketManager::destroySSL() {
	_ssl = false;
	ERR_free_strings();
	EVP_cleanup();
}

SSL_CTX*	socketManager::createSSLContext(struct socketParameter &params) {
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_server_method());
	if (!ctx)
		throw std::runtime_error("socketManager::createSSLContext:	SSL_CTX_new");

	if (SSL_CTX_use_certificate_file(ctx, params.sslCertificate.c_str(), SSL_FILETYPE_PEM) <= 0) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	SSL_CTX_use_certificate_file");
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, params.sslKey.c_str(), SSL_FILETYPE_PEM) <= 0) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	SSL_CTX_use_PrivateKey_file");
	}

	if (SSL_CTX_check_private_key(ctx) != 1) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	SSL_CTX_check_private_key");
	}

	if (!SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3)) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	Couldn't set SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3");
	}

	if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	Couldn't set min proto version to TLS1_2_VERSION");
	}

	if (!SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION)) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	Couldn't set max proto version to TLS1_3_VERSION");
	}

	if (!SSL_CTX_set_options(ctx, SSL_OP_CIPHER_SERVER_PREFERENCE)) {
		SSL_CTX_free(ctx);
		throw std::runtime_error("socketManager::createSSLContext:	Couldn't set SSL_OP_CIPHER_SERVER_PREFERENCE");
	}

	if (params.sslCiphers.size() > 0) {
		std::string ciphers;
		for (std::vector<std::string>::iterator it = params.sslCiphers.begin(); it != params.sslCiphers.end(); it++) {
			ciphers += *it;
			if (it + 1 != params.sslCiphers.end())
				ciphers += ":";
		}
		if (SSL_CTX_set_cipher_list(ctx, ciphers.c_str()) != 1) {
			SSL_CTX_free(ctx);
			throw std::runtime_error("socketManager::createSSLContext:	SSL_CTX_set_cipher_list");
		}
	}
	return (ctx);
}

void		socketManager::SSLAccept(int fd) {
	// std::cout << "socketManager::SSLAccept: for fd: " << fd << std::endl;
	int	acceptReturn = SSL_accept((SSL*)SOCKET.info.sslData.Context);
	int	error = SSL_get_error((SSL*)SOCKET.info.sslData.Context, acceptReturn);
	switch (acceptReturn) {
		case -1:
			switch (error) {
				case SSL_ERROR_SSL: // 1
					SOCKET.info.sslData.established = false;
					SOCKET.info.sslData.read = true;
					SOCKET.info.sslData.write = false;
					throw std::runtime_error("socketManager::SSLAccept:	Certificate is not trusted or is self-signed");
				case SSL_ERROR_WANT_READ:	// 2
					SOCKET.info.sslData.read = true;
					SOCKET.info.sslData.write = false;
					return ;
				case SSL_ERROR_WANT_WRITE:	// 3
					SOCKET.info.sslData.write = true;
					SOCKET.info.sslData.read = false;
					return ;
				default:
					std::cout << "socketManager::SSLAccept:	SSL_accept returned with error code: " << error << std::endl;
					std::cout << "socketManager::SSLAccept:	" << error << " means: ";
					ERR_print_errors_fp(stdout);
					removeSocket(fd);
					return ;
			}
		case 0:
			return ;
		case 1:
			// std::cout << "SSL accept success" << std::endl;
			SOCKET.info.sslData.established = true;
			SOCKET.info.sslData.read = false;
			SOCKET.info.sslData.write = false;
			return ;
		case 2:
			std::cout << "socketManager::SSLAccept:	SSL_accept returned with error code: " << error << std::endl;
			std::cout << "socketManager::SSLAccept:	" << error << " means: ";
			ERR_print_errors_fp(stdout);
			removeSocket(fd);
			return ;
		default:
			std::cout << "socketManager::SSLAccept:	SSL_accept returned with error code: " << error << std::endl;
			std::cout << "socketManager::SSLAccept:	" << error << " means: ";
			ERR_print_errors_fp(stdout);
			removeSocket(fd);
			return ;
	}
	return ;
}
