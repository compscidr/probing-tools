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
#include <sstream>
#include <thread>
#include "tcp_scanner.h"

constexpr int MAX_OUTSTANDING = 1024;

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
    in_addr_t sa = inet_addr(start_address.c_str());
    in_addr_t ea = inet_addr(end_address.c_str());
    fd_set wrfds;
    FD_ZERO(&wrfds);
    sa = ntohl(sa);
    ea = ntohl(ea);
    int count = 0;
    int n = 0;
    int outstanding = 0;

    while(sa <= ea) {
        struct hostent * server;
        server = gethostbyname(start_address.c_str());
        if (server == nullptr) {
          std::cout << "coudln't resolve: " << start_address << std::endl;
          continue;
        }

        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

        int s = start_port;

        while(s <= end_port) {
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            outstanding++;
            if (sockfd < 0) {
                std::stringstream ss;
                ss << "Error opening socket to " << start_address << ":" << s;
                throw std::runtime_error(ss.str());
            }
            fcntl(sockfd, F_SETFL, O_NONBLOCK);
            count++;
            serv_addr.sin_port = htons(s);

            scan_map[sockfd] = std::make_shared<tcp_scan>(s, start_address, sockfd);
            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
                //we expect this to happen
                if(errno == EINPROGRESS) {
                    //std::cout << "Connect in progress to " << start_address << ":" << s << std::endl;
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

            if (outstanding >= MAX_OUTSTANDING) {
              wait_for_connects(wrfds, outstanding);
              outstanding = 0;
              FD_ZERO(&wrfds);
            }
        }
        start_address = std::string(increment_address(start_address.c_str()));
        sa = inet_addr(start_address.c_str());
        sa = ntohl(sa);
    }

    wait_for_connects(wrfds, outstanding);
}

void tcp_scanner::wait_for_connects(fd_set wrfds, int outstanding) {
  //std::cout << "WAITING FOR " << outstanding << " requests" << std::endl;
  struct timeval tout{0, 500000};
  fd_set wrfds_orig = wrfds;
  int n = 0;

  while(n != outstanding) {
    int error = select(FD_SETSIZE, NULL, &wrfds, NULL, &tout);
    if (error == 0) {
      // timeout, return immediately and give up on everything that has timed out
      //std::cout << "timeout" << std::endl;
      for (auto it : scan_map) {
        close(it.first);
      }
      scan_map.erase(scan_map.begin(), scan_map.end());
      return;
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

      if (getsockopt(it.first, SOL_SOCKET, SO_ERROR, &optval, &optlen) == -1)
        err(1, "getsockopt");

      if (optval == 0) {
        //std::cout << "Connected to " << it.second->get_host() << ":" << it.second->get_port() << std::endl;
        it.second->success();
        success_list.push_back(*it.second);
      } else {
        //std::cout << "Failed connect to " << it.second->get_host() << ":" << it.second->get_port() << std::endl;
      }
      FD_CLR(it.first, &wrfds_orig);
      ++n;
    }
    wrfds = wrfds_orig;
  }

  for (auto it : scan_map) {
    close(it.first);
  }
  scan_map.erase(scan_map.begin(), scan_map.end());
}