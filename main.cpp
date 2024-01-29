#include <array>
#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;

int main()
{
    try
    {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(8080));
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::error_code ec;
        size_t len = socket.write_some(asio::buffer(std::string("request-AA")), ec);
        if (ec)
        {
            std::cout << ec.message() << std::endl;
            return 0;
        }
        std::cout << "wrote on socket N bytes : " << len << std::endl;
        for (;;)
        {
            std::cout << "Reading from buffer " << std::endl;
            std::array<char, 128> buf;
            asio::error_code error;

            size_t len = socket.read_some(asio::buffer(buf), error);
            std::cout << "message length: " << len << std::endl;
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
            std::cout << std::endl;
        }
    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}