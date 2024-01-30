#include "../logic/HorcruxData.hpp"
#include "../networking/Requests.hpp"

#include <array>
#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;
using namespace Horcrux;

std::vector<HorcruxData> getSampleHorcrux(const std::string& path, Horcrux::Client::FileSplitter* splitter)
{
    auto binaries = Utils::loadFile(path);
    if (binaries.has_value())
        return splitter->split(binaries.value());
    else
        return {};
}

bool readResponse(tcp::socket& socket)
{
    Utils::log("Reading server response");
    std::array<char, 128> readBuf{};
    asio::error_code error;

    size_t len = socket.read_some(asio::buffer(readBuf), error);
    Utils::log("Received message with length: ", len);

    if (error)
        throw asio::system_error(error); // Some other error.

    std::string response(readBuf.data(), readBuf.data() + len);
    Utils::log("Received response is: ", response);
    if (response == "SUCCESS")
        return true;

    if (response == "FAILURE")
        return false;

    return false;
}

bool sendHorcruxRequest(tcp::socket& socket, const Save::Request& request)
{
    std::error_code ec;
    auto serializedHeader = Utils::serializeHeader(request.mHeader);
    Utils::log("Sending request header: ", request.mHeader);
    size_t len = socket.write_some(asio::buffer(serializedHeader), ec);
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        return false;
    }

    Utils::log("Written header on socket with size: ", len);

    len = socket.write_some(asio::buffer(request.mContent), ec);
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        return false;
    }

    if (!readResponse(socket))
        return false;

    Utils::log("Written Horcrux content on socket with size: ", len);
    return true;
}

int main(int argc, char** argv)
{
    std::optional<Utils::ClientSaveOptions> options = Utils::parseHorcruxCommand(argc, argv);
    if (!options.has_value())
        return -1;

    try
    {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(8080));
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::unique_ptr<Horcrux::Client::FileSplitter> splitter = std::make_unique<Client::BasicFileSplitter>(options->mHorcruxCount);
        std::vector<HorcruxData> horcruxes = getSampleHorcrux(options->mInputPath, splitter.get());

        if (horcruxes.empty())
            return -1; // check log

        Save::PreRequestHeader preRequestHeader = Save::PreRequestHeader::fromHorcruxes(Utils::generateRandomUint64(), horcruxes);
        auto serializedPreRequestHeader = Utils::serializeHeader(preRequestHeader);

        std::error_code ec;
        size_t len = socket.write_some(asio::buffer(serializedPreRequestHeader, serializedPreRequestHeader.size()), ec);
        if (ec)
        {
            std::cout << ec.message() << std::endl;
            return 0;
        }
        Utils::log("Sent N bytes to socket: ", len);

        if (!readResponse(socket))
            return 0;

        auto requests = Save::Request::fromHorcruxes(preRequestHeader.mUuid, std::move(horcruxes));
        for (auto& request : requests)
        {
            Utils::log("#### Sending Horcrux with id: ", request.mHeader.mIndex);
            bool ok = sendHorcruxRequest(socket, request);
            if (!ok)
                Utils::log("Something went wrong while shipping stuff");
            Utils::log("#### End message");
        }

    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}