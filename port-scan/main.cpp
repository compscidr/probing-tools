#include <iostream>
#include "udp_scanner.h"

int main() {

    udp_scanner udp;
    udp.scan("10.0.0.116", "10.0.0.118", 1001, 1005, "test", 4);

    return 0;
}
