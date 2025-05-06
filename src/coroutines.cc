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

std::atomic<uint64_t> processed = 0;

struct thread_setup {
    static std::atomic<uint64_t> atm_thread_id;
    thread_setup() {
        uint64_t thread_id = atm_thread_id++;
        std::cout << "setting up thread " << thread_id << std::endl;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(thread_id, &cpuset);
        auto th = pthread_self();
        int rc = pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpuset);

        sched_param sparams;
        sparams.sched_priority = 10;
        rc = pthread_setschedparam(th, SCHED_FIFO, &sparams);
    }
};
std::atomic<uint64_t> thread_setup::atm_thread_id;

/*asio::awaitable<void>*/ void process_packet(const packet_buf pb) {
    thread_local uint64_t processed_accumulator = 0;
    thread_local thread_setup ts{};
    packet p = std::bit_cast<packet>(pb);
    //auto end = std::chrono::steady_clock::now() + 1s;
    //while (std::chrono::steady_clock::now() < end) {
    //}
    processed_accumulator++;
    if (processed_accumulator == 1024) {
        processed += processed_accumulator;
        processed_accumulator = 0;
    }
    //co_return;
}

asio::awaitable<void> receive_packets(udp::socket socket, asio::thread_pool& thread_pool) {
    auto io_context = co_await asio::this_coro::executor;
    while (true) {
        packet_buf p{};
        auto [e1, n] = co_await socket.async_receive(asio::buffer(p), use_nothrow_awaitable);
        if (e1) {
            break;
        }
        std::bit_cast<packet>(p).check_sequence_number();
        //asio::co_spawn(io_context, [p]() -> auto { return process_packet(p); }, asio::detached);
        asio::post(thread_pool, [p]() { process_packet(p); });
    }
}

asio::awaitable<void> stats_printer() {
    auto io_context = co_await asio::this_coro::executor;
    asio::steady_timer timer(io_context);
    do {
        timer.expires_from_now(1s);
        co_await timer.async_wait(use_nothrow_awaitable);
        std::cout << "processed packets " << processed << std::endl;
    } while (true);
}

int main(int argc, char* argv[]) {
    unsigned num_cores = std::thread::hardware_concurrency() / 2 - 1;

    asio::thread_pool thread_pool(num_cores);
    asio::io_context io_context;

    asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { io_context.stop(); });
    asio::co_spawn(
        io_context, receive_packets(setup_socket(io_context, false, "0.0.0.0", "62040"), thread_pool), asio::detached
    );

    asio::co_spawn(io_context, stats_printer, asio::detached);

    io_context.run();
    return 0;
}
