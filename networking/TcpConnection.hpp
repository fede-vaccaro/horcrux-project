//
// Created by federico on 1/30/24.
//

#ifndef HORCRUX_NETWORKING_TCPCONNECTION_HPP_
#define HORCRUX_NETWORKING_TCPCONNECTION_HPP_

#include "../networking/Requests.hpp"

#include <asio.hpp>
#include <iostream>
#include <memory>

using asio::ip::tcp;
namespace Horcrux {
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    struct Config
    {
        uint64_t mMaxNumHorcruxAccepted = 20;   //  if the client requests to save too many horcruxes, the entire request is rejected
        uint64_t mMaxTotalSizeAccepted = 10000; // if an horcrux goes over this size, the entire request is rejected
    };

    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    static TcpConnectionPtr createNew(asio::io_context& ioContext) { return TcpConnectionPtr(new TcpConnection(ioContext)); }

    tcp::socket& socket() { return mTcpSocket; }

    void Start() { ReadPreRequestHeader(); }

private:
    TcpConnection(asio::io_context& ioContext) : mTcpSocket(ioContext) {}

    void ReadPreRequestHeader();

    void ReadRequestHeader();

    void ReadRequestContent();

    void writeHandler(const asio::error_code& ec, size_t byteTransferred);

    bool checkPreRequestHeader() const;
    bool checkRequestHeader() const;
    bool saveHorcruxes();

    template <typename OP>
    void sendResponse(bool outcome, OP&& NEXT_OP)
    {
        if (outcome)
            mMessageBuffer = "SUCCESS";
        else
            mMessageBuffer = "FAILURE";

        Utils::log("Sending response to client: ", mMessageBuffer);
        auto self = shared_from_this();
        asio::async_write(mTcpSocket, asio::buffer(mMessageBuffer), [self, this, NEXT_OP](asio::error_code ec, std::size_t length) {
            writeHandler(ec, length);
            NEXT_OP();
        });
    }

    // app config
    Config mConfig{};

    // networking stuff
    tcp::socket mTcpSocket;

    // data buffers
    std::array<char, sizeof(Save::PreRequestHeader)> mPreRequestHeaderBuffer{};
    Save::PreRequestHeader mPreRequestHeader{};

    std::array<char, sizeof(Save::Request::Header)> mRequestHeaderBuffer{};
    Save::Request::Header mRequestHeader{};

    std::string mMessageBuffer{};

    // Here is where the server is going to store the received Horcruxes
    // std::optional help checking the sanity of the client's data
    std::vector<std::optional<HorcruxData>> mCurrentRequestHorcruxes;
};
} // namespace Horcrux

#endif // HORCRUX_NETWORKING_TCPCONNECTION_HPP_
