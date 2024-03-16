/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interface.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 12:01:41 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/11 19:32:45 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interface.hpp"

std::map<uint32_t, protocolFunction>	Interface::_protocolMap;
std::map<int, std::string>				Interface::_outputBuffer;

void	Interface::interface(int sock, struct sockData data) {
	// std::cout << "interface called" << std::endl;
	if (data.READ) {
		socketManager::detectActivity(sock);
		std::string		request;
		std::string		response;
		if (readFromSocket(sock, data, request)) {
			std::cout << "readFromSocket failed" << std::endl;
			socketManager::removeSocket(sock);
			return ;	// remove client
		}
		if (passRequest(request, response, data.info.port)) {
			std::cout << "passRequest failed" << std::endl;
			socketManager::removeSocket(sock);
			return ;	// remove client
		}
		std::cout << "response added to buffer for socket: " << sock << std::endl;
		_outputBuffer.insert(std::pair<int, std::string>(sock, response));
	}
	else if (data.WRITE && _outputBuffer.find(sock) != _outputBuffer.end()) {
		socketManager::detectActivity(sock);
		std::cout << "writing to socket: " << sock << std::endl;
		std::string	response = _outputBuffer[sock];
		if (writeToSocket(sock, data, response)) {
			std::cout << "writeToSocket failed" << std::endl;
			socketManager::removeSocket(sock);
			_outputBuffer.erase(sock);
			return ;	// remove client
		}
		_outputBuffer.erase(sock);
	}
}

void	Interface::addProtocol(uint32_t port, protocolFunction function) {
	_protocolMap.insert(std::pair<uint32_t, protocolFunction>(port, function));
}
