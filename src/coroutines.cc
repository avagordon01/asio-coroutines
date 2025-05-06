#include <iostream>
#include <queue>
#include <vector>
#include <chrono>

#define ASIO_HAS_IO_URING 1
#define ASIO_DISABLE_EPOLL 1
#include <asio.hpp>

#include "util.hh"
#include "packet.hh"

using asio::ip::udp;
using namespace std::chrono_literals;

std::queue<packet_buf> packet_queue;

asio::awaitable<void> process_packets() {
    while (!packet_queue.empty()) {
        std::cout << "processing packet" << std::endl;
        packet p = std::bit_cast<packet>(packet_queue.front());
        packet_queue.pop();
        p.check_sequence_number();
        std::cout << "sequence number " << p.sequence_number << std::endl;
        std::cout << packet_queue.size() << " packets in queue" << std::endl;
    }
    co_return;
}

asio::awaitable<void> receive_packets(std::string id, udp::socket socket) {
    auto io_context = co_await asio::this_coro::executor;
    while (true) {
        packet_buf buffer{};
        auto [e1, n] = co_await socket.async_receive(asio::buffer(buffer), use_nothrow_awaitable);
        if (e1) {
            break;
        }
        std::cout << "received packet on coroutine " << id << std::endl;
        packet_queue.emplace(std::move(buffer));
        std::cout << packet_queue.size() << " packets in queue" << std::endl;
        asio::co_spawn(io_context, process_packets(), asio::detached);
    }
}

int main(int argc, char *argv[]) {
    unsigned num_cores = std::thread::hardware_concurrency();
    asio::io_context io_context(num_cores);

    asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { io_context.stop(); });
    asio::co_spawn(io_context, receive_packets("a", setup_socket(io_context, false, "0.0.0.0", "62040")), asio::detached);
    asio::co_spawn(io_context, receive_packets("b", setup_socket(io_context, false, "0.0.0.0", "62041")), asio::detached);

    io_context.run();
    return 0;
}
