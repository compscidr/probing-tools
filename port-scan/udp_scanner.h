//
// Created by jason on 2020-06-26.
//

#ifndef PORT_SCAN_UDP_SCANNER_H
#define PORT_SCAN_UDP_SCANNER_H


class udp_scanner {
public:
    udp_scanner();
    void scan(std::string start_address, std::string end_address, int start_port, int end_port, const char *payload, const int payload_len);
    ~udp_scanner();
};


#endif //PORT_SCAN_UDP_SCANNER_H
