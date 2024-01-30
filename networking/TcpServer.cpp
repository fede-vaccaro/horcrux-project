//
// Created by federico on 1/30/24.
//

#include "TcpServer.hpp"

namespace Horcrux {
void TcpServer::handle_accept(TcpConnection::TcpConnectionPtr newConnection, const asio::error_code& error)
{
    if (!error)
    {
        newConnection->Start();
    }

    startAccept();
}

void TcpServer::startAccept()
{
    TcpConnection::TcpConnectionPtr newConnection = TcpConnection::createNew(mIoContext);

    mTcpAcceptor.async_accept(newConnection->socket(), [this, newConnection](std::error_code ec) { handle_accept(newConnection, ec); });
}
} // namespace Horcrux