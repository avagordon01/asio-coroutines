#include <iostream>
#include <thread>
#include <chrono>

#define ASIO_HAS_IO_URING 1
#define ASIO_DISABLE_EPOLL 1
#include <asio.hpp>

#include "util.hh"
#include "packet.hh"

using asio::ip::udp;

void usage() {
    std::cout << "usage: broadcast address port" << std::endl;
    std::exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        usage();
    }
    std::string address = argv[1];
    std::string port = argv[2];
    try {
        asio::io_context io_context;

        udp::socket connection = setup_socket(io_context, false, address, port);
        while (true) {
            packet_buf data;
            std::size_t n = connection.receive(asio::buffer(data));
            std::cout << "received packet" << std::endl;
        }
    } catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
