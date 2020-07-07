#include <iostream>
#include "udp_scanner.h"
#include "tcp_scanner.h"

int main() {

    //udp_scanner udp;
    //udp.scan("10.0.0.116", "10.0.0.116", 5000, 5001, "test", 4);

    tcp_scanner tcp;
    tcp.scan("10.0.0.116", "10.0.0.116", 80, 80);

    return 0;
}
