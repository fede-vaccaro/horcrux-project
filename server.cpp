#include <asio.hpp>
#include <iostream>
#include <memory>

using asio::ip::tcp;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    static TcpConnectionPtr createNew(asio::io_context& ioContext) { return TcpConnectionPtr(new TcpConnection(ioContext)); }

    tcp::socket& socket() { return mTcpSocket; }

    void Start() { Read(); }

private:
    TcpConnection(asio::io_context& ioContext) : mTcpSocket(ioContext) {}

    void Write()
    {
        auto self(shared_from_this());
        asio::async_write(mTcpSocket, asio::buffer(mMessageBuffer), [self, this](asio::error_code ec, std::size_t length) { handle_write(ec, length); });
    }

    void Read()
    {
        auto self(shared_from_this());
        asio::async_read(mTcpSocket, asio::buffer(self->mRequestBuffer), [this, self](const asio::error_code& ec, size_t length) {
            if (ec != asio::error::eof)
            {
                std::cout << "[SERVER] some error occurred: " << ec.message() << " | received message of length: " << length << std::endl;
            }
            std::cout << "received X bytes : " << length << " " << std::endl;
            std::cout.write(mRequestBuffer.data(), length);
            std::cout << std::endl;
            std::string requestString(mRequestBuffer.begin(), mRequestBuffer.end());
            if (requestString == "request-AA")
                mMessageBuffer = "received request is 'request-A'";
            else if (requestString == "request-BB")
                mMessageBuffer = "received request is 'request-B'";
            else
                mMessageBuffer = "INVALID REQUEST";
            //
            //                       std::cout << " mRequestBuffer" << mRequestBuffer
            //                                 << std::endl;
            //
            Write();
        });
    }

    void handle_write(const asio::error_code& ec, size_t byteTransferred)
    {
        if (!ec)
        {
            std::cout << "[Server] Ok - has transferred " << byteTransferred << "bytes \n";
            return;
        }
        std::cout << "[Server] something went wrong " << ec.message() << std::endl;
    }

    tcp::socket mTcpSocket;
    std::string mMessageBuffer;
    std::array<char, 10> mRequestBuffer;
};

class TcpServer
{
public:
    static constexpr uint32_t kServerPort = 8080;

    TcpServer(asio::io_context& ioContext) : mIoContext(ioContext), mTcpAcceptor(ioContext, tcp::endpoint(tcp::v4(), kServerPort)) { startAccept(); }

    void startAccept()
    {
        TcpConnection::TcpConnectionPtr newConnection = TcpConnection::createNew(mIoContext);

        mTcpAcceptor.async_accept(newConnection->socket(), [this, newConnection](std::error_code ec) { handle_accept(newConnection, ec); });
    }

    void handle_accept(TcpConnection::TcpConnectionPtr newConnection, const asio::error_code& error)
    {
        if (!error)
        {
            newConnection->Start();
        }

        startAccept();
    }

private:
    asio::io_context& mIoContext;
    asio::ip::tcp::acceptor mTcpAcceptor;
};

int main()
{
    try
    {
        asio::io_context io_context;
        TcpServer server(io_context);
        io_context.run();
    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
