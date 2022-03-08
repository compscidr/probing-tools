//
// Created by jason on 7/6/20.
//

#ifndef PROBING_TOOLS_TCP_SCANNER_H
#define PROBING_TOOLS_TCP_SCANNER_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>

class tcp_scanner {
    class tcp_scan {
        int port;
        std::string host;
        int sock_fd;
        bool result;
    public:
        tcp_scan(int port, std::string host, int sock_fd) : port(port), host(std::move(host)), sock_fd(sock_fd), result(false) {};
        int get_port() const { return port; }
        std::string get_host() const { return host; }
        int get_sockfd() const { return sock_fd; }
        bool get_result() const { return result; }
        void success() { result = true; }
    };
public:
    void scan(std::string start_address, std::string end_address, int start_port, int end_port);
    std::vector<tcp_scan> get_successful() { return success_list; }
private:
    void wait_for_connects(fd_set wrfds, int outstanding);
    //mapping of sockfd -> tcp_scan
    std::unordered_map<int, std::shared_ptr<tcp_scan>> scan_map;
    std::vector<tcp_scan> success_list;
};


#endif //PROBING_TOOLS_TCP_SCANNER_H
