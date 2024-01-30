#include "networking/TcpServer.hpp"

#include <asio.hpp>
#include <iostream>
#include <memory>

using asio::ip::tcp;

int main()
{
    try
    {
        asio::io_context io_context;
        Horcrux::TcpServer server(io_context);
        io_context.run();
    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
