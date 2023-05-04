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

        udp::socket connection = setup_socket(io_context, true, address, port);
        while (true) {
            packet p{};
            connection.send(asio::buffer(std::bit_cast<packet_buf>(p)));
            //using namespace std::chrono_literals;
            //std::this_thread::sleep_for(1s);
        }
    } catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
