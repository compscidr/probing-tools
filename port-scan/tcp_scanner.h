//
// Created by jason on 7/6/20.
//

#ifndef PROBING_TOOLS_TCP_SCANNER_H
#define PROBING_TOOLS_TCP_SCANNER_H

#include <string>

class tcp_scanner {
public:
    void scan(std::string start_address, std::string end_address, int start_port, int end_port);
};


#endif //PROBING_TOOLS_TCP_SCANNER_H
