//
// Created by jason on 7/6/20.
//

#include <sys/socket.h>
#include <stdexcept>

#include <netdb.h>
#include <strings.h>
#include <iostream>
#include <zconf.h>
#include "tcp_scanner.h"

void tcp_scanner::scan(std::string start_address, std::string end_address, int start_port, int end_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Error opening socket");
    }

    struct hostent * server;
    server = gethostbyname(start_address.c_str());
    if (server == nullptr) {
        throw std::runtime_error("Error resolving " + start_address);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(start_port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Failed to connect to " << start_address << ":" << start_port << std::endl;
    } else {
        std::cout << "Connected to " << start_address << ":" << start_port << std::endl;
        close(sockfd);
    }

}