#pragma once

#include <boost/asio.hpp>

namespace asio = boost::asio;

asio::ip::udp::socket setup_socket(asio::io_context& io_context, bool send, std::string ip, std::string port) {
    using asio::ip::udp;

    udp::resolver resolver(io_context);
    udp::endpoint endpoint = *resolver.resolve(udp::v4(), ip.c_str(), port.c_str()).begin();
    udp::socket socket(io_context);
    socket.open(udp::v4());
    if (send) {
        socket.connect(endpoint);
    } else {
        socket.bind(endpoint);
    }
    socket.set_option(asio::socket_base::reuse_address{true});
    socket.set_option(asio::socket_base::broadcast{true});
    return socket;
}

constexpr auto use_nothrow_awaitable = asio::as_tuple(asio::use_awaitable);
