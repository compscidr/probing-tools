//
// Created by jason on 7/6/20.
//

#ifndef PROBING_TOOLS_TCP_SCANNER_H
#define PROBING_TOOLS_TCP_SCANNER_H

#include <string>
#include <unordered_map>

class tcp_scanner {
    class tcp_scan {
        int port;
        std::string host;
        int sockfd;
    public:
        tcp_scan(int port, std::string host, int sockfd) {this->port = port; this->host = host; this->sockfd = sockfd; }
        int get_port() const { return port; }
        std::string get_host() const { return host; }
        int get_sockfd() const { return sockfd; }
    };
public:
    void scan(std::string start_address, std::string end_address, int start_port, int end_port);
private:
    //mapping of sockfd -> tcp_scan
    std::unordered_map<int, tcp_scan> scan_map;
};


#endif //PROBING_TOOLS_TCP_SCANNER_H
