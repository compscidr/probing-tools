//
// Created by jason on 2020-06-26.
//

#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include "udp_scanner.h"

udp_scanner::udp_scanner() {
}

void udp_scanner::scan(std::string start_address, std::string end_address, int start_port, int end_port, const char *payload, const int payload_len) {
    std::cout << "Starting scan from " << start_address << "-" << end_address << ":" << start_port << "-" << end_port << std::endl;

    sockaddr_in start_sockaddr_in;
    sockaddr_in end_sockaddr_in;

    if(!inet_aton(start_address.c_str(), &start_sockaddr_in.sin_addr)) {
        throw std::runtime_error("Invalid starting address: " + start_address);
    }
    if(!inet_aton(end_address.c_str(), &end_sockaddr_in.sin_addr)) {
        throw std::runtime_error("Invalid ending address: " + end_address);
    }

    int send_socket = socket(AF_UNSPEC, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

    while (start_sockaddr_in.sin_addr.s_addr <= end_sockaddr_in.sin_addr.s_addr) {
        std::cout << "Scanning: " << inet_ntoa(start_sockaddr_in.sin_addr) << std::endl;

        int current_port = start_port;
        while (current_port <= end_port) {
            std::cout << "  Scanning port: " << current_port << std::endl;
            start_sockaddr_in.sin_family = AF_INET;
            start_sockaddr_in.sin_port = current_port;
            if(!sendto(send_socket, payload, payload_len, 0, (struct sockaddr*)&start_sockaddr_in, sizeof(start_sockaddr_in))) {
                throw std::runtime_error("Failed sending UDP packet to "
                    + std::string(inet_ntoa(start_sockaddr_in.sin_addr)) + ":" + std::to_string(current_port));
            }

            current_port++;
        }

        int temp = ntohl(start_sockaddr_in.sin_addr.s_addr) + 1;
        start_sockaddr_in.sin_addr.s_addr = htonl(temp);
    }
}

udp_scanner::~udp_scanner() {

}