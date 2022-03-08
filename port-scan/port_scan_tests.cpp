#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <atomic>
#include <thread>
#include "tcp_scanner.h"

constexpr int TEST_PORT = 8080;

TEST(PortScanTest, DummyTest) {
  ASSERT_TRUE(true);
}

TEST(PortScanTest, NotListening) {
  tcp_scanner tcpScanner;
  tcpScanner.scan("127.0.0.1", "127.0.0.1", TEST_PORT, TEST_PORT);
  auto successes = tcpScanner.get_successful();

  ASSERT_EQ(successes.size(), 0);
}

TEST(PortScanTest, LocalSinglePortTest) {
  // setup a server to listen on a particular port
  int opt = 1;
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
             &opt, sizeof(opt));
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(TEST_PORT);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3);

  std::atomic<bool> listening{true};
  std::thread t([&listening, &server_fd](){
    while (listening) {
      struct sockaddr_in client_address;
      int addrlen = sizeof(client_address);
      int new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
      std::cout << "Accepted connection" << std::endl;
      close(new_socket);
    }
  });

  // run the scan
  tcp_scanner tcpScanner;
  tcpScanner.scan("127.0.0.1", "127.0.0.1", TEST_PORT, TEST_PORT);
  auto successes = tcpScanner.get_successful();

  listening = false;
  shutdown(server_fd, SHUT_RD);
  close(server_fd);
  t.join();

  // make sure we got a single result
  ASSERT_EQ(successes.size(), 1);
}

TEST (PortScanTest, ManyPorts) {
  tcp_scanner tcpScanner;
  tcpScanner.scan("10.0.0.1", "10.0.0.1", 1, 65000);
  // don't assert anything about ports because who knows what is actually open
  // as long as we don't crash we're good.
}

TEST (PortScanTest, ManyHosts) {
  tcp_scanner tcpScanner;
  tcpScanner.scan("10.0.0.1", "10.0.0.250", 80, 80);

  // don't assert anything about ports because who knows what is actually open
  // as long as we don't crash we're good.
}