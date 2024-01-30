//
// Created by federico on 1/30/24.
//

#ifndef HORCRUX_NETWORKING_TCPSERVER_HPP_
#define HORCRUX_NETWORKING_TCPSERVER_HPP_

#include "TcpConnection.hpp"

#include <asio.hpp>
#include <cstdint>
#include <iostream>
#include <memory>

using asio::ip::tcp;

namespace Horcrux {
class TcpServer
{
public:
    static constexpr uint32_t kServerPort = 8080;

    TcpServer(asio::io_context& ioContext) : mIoContext(ioContext), mTcpAcceptor(ioContext, tcp::endpoint(tcp::v4(), kServerPort)) { startAccept(); }

    void startAccept();

    void handle_accept(TcpConnection::TcpConnectionPtr newConnection, const asio::error_code& error);

private:
    asio::io_context& mIoContext;
    asio::ip::tcp::acceptor mTcpAcceptor;
};
} // namespace Horcrux
#endif // HORCRUX_NETWORKING_TCPSERVER_HPP_
