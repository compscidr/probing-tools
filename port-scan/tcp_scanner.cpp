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
#include <fcntl.h>
#include <sys/poll.h>
#include <err.h>
#include <cstring>
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

//influenced by: http://mff.devnull.cz/pvu/src/tcp/non-blocking-connect.c
void tcp_scanner::scan(std::string start_address, std::string end_address, int start_port, int end_port) {
    int success = 0;
    in_addr_t sa = inet_addr(start_address.c_str());
    in_addr_t ea = inet_addr(end_address.c_str());
    fd_set wrfds, wrfds_orig;
    struct timeval tout;
    FD_ZERO(&wrfds);
    sa = ntohl(sa);
    ea = ntohl(ea);
    int count = 0;
    int n = 0;
    while(sa <= ea) {
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
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                throw std::runtime_error("Error opening socket");
            }
            fcntl(sockfd, F_SETFL, O_NONBLOCK);
            count++;
            serv_addr.sin_port = htons(s);

            std::pair<int, tcp_scan> pair(sockfd, tcp_scan(s, start_address, sockfd));
            scan_map.insert(pair);
            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
                //we expect this to happen
                if(errno == EINPROGRESS) {
                    //std::cout << "Connect in progress" << std::endl;
                    FD_SET(sockfd, &wrfds);
                } else {
                    //may happen on localhost
                    std::cout << "Connect error: " << strerror(errno) << std::endl;
                    ++n;
                }
            } else {
                //may happen on localhost
                std::cout << "Connect error: " << strerror(errno) << std::endl;
                ++n;
            }
            s++;
        }
        start_address = std::string(increment_address(start_address.c_str()));
        sa = inet_addr(start_address.c_str());
        sa = ntohl(sa);
    }

    wrfds_orig = wrfds;
    tout.tv_sec = 10;
    tout.tv_usec = 0;
    int timeouts = 0;

    while(n != count) {
        int error = select(FD_SETSIZE, NULL, &wrfds, NULL, &tout);
        if (error == 0) {
            timeouts++;
            std::cout << "Timeout" << std::endl;
            tout.tv_sec = 1;
            tout.tv_usec = 0;
            continue;
        } else {
            if (error == -1)
                err(1, "select");
        }

        for (auto it : scan_map) {
            if (!FD_ISSET(it.first, &wrfds)) {
                continue;
            }

            int optval = -1;
            socklen_t optlen = sizeof(optval);

            if (getsockopt(it.second.get_sockfd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) == -1)
                err(1, "getsockopt");

            if (optval == 0) {
                success++;
                std::cout << "Connected to " << it.second.get_host() << ":" << it.second.get_port() << std::endl;
            } else {
                //std::cout << "Failed connect to " << it.second.get_host() << ":" << it.second.get_port() << std::endl;
            }
            FD_CLR(it.first, &wrfds_orig);
            close(it.second.get_sockfd());
            ++n;
        }
        wrfds = wrfds_orig;
    }

    std::cout << "Success: " << success << std::endl;
}