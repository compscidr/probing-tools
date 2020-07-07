//
// Created by jason on 7/6/20.
//

#include <sys/socket.h>
#include <stdexcept>

#include <netdb.h>
#include <strings.h>
#include <iostream>
#include <zconf.h>
#include <arpa/inet.h>
#include "tcp_scanner.h"

// NOTE: only works for IPv4.  Check out inet_pton/inet_ntop for IPv6 support.
// https://stackoverflow.com/questions/1505676/how-do-i-increment-an-ip-address-represented-as-a-string
char* increment_address(const char* address_string)
{
    // convert the input IP address to an integer
    in_addr_t address = inet_addr(address_string);

    // add one to the value (making sure to get the correct byte orders)
    address = ntohl(address);
    address += 1;
    address = htonl(address);

    // pack the address into the struct inet_ntoa expects
    struct in_addr address_struct;
    address_struct.s_addr = address;

    // convert back to a string
    return inet_ntoa(address_struct);
}

void tcp_scanner::scan(std::string start_address, std::string end_address, int start_port, int end_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Error opening socket");
    }

    int i = 0;
    while(start_address <= end_address) {
        struct hostent * server;
        server = gethostbyname(start_address.c_str());
        if (server == nullptr) {
            throw std::runtime_error("Error resolving " + start_address);
        }

        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

        int s = start_port;

        while(s <= end_port) {
            serv_addr.sin_port = htons(s);

            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                std::cout << "Failed to connect to " << start_address << ":" << s << std::endl;
            } else {
                std::cout << "Connected to " << start_address << ":" << s << std::endl;
                close(sockfd);
            }
            s++;
        }
        start_address = std::string(increment_address(start_address.c_str()));
    }
}