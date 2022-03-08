#include <iostream>
#include "udp_scanner.h"
#include "tcp_scanner.h"

int main() {

    //udp_scanner udp;
    //udp.scan("10.0.0.116", "10.0.0.116", 5000, 5001, "test", 4);

    tcp_scanner tcp;
    tcp.scan("10.0.0.1", "10.0.0.1", 1, 65000);

    for (const auto& scan : tcp.get_successful()) {
      std::cout << scan.get_host() << ":" << scan.get_port() << std::endl;
    }

    return 0;
}
