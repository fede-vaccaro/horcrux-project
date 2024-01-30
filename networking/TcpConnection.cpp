//
// Created by federico on 1/30/24.
//

#include "TcpConnection.hpp"

#include "Utils.hpp"

#include <regex>

namespace Horcrux {

bool TcpConnection::saveHorcruxes()
{
    Utils::log("Total number of received Horcruxes: ", mCurrentRequestHorcruxes.size());
    uint32_t counter = 0;
    for (const std::optional<HorcruxData>& hcx : mCurrentRequestHorcruxes)
    {
        std::filesystem::path horcruxPath{ std::to_string(mPreRequestHeader.mUuid) + ".hcx." + std::to_string(counter) };
        Utils::saveFile(hcx->mContent, kOutPath / horcruxPath);
        counter++;
    }
    return true;
}

bool TcpConnection::checkPreRequestHeader() const
{
    std::regex regexPattern(R"(\d+\.hcx\.\d+)");

    for (const auto& entry : std::filesystem::directory_iterator(kOutPath))
    {
        if (std::filesystem::is_regular_file(entry.path()) && std::regex_match(entry.path().filename().string(), regexPattern))
        {
            Utils::log("The client is requesting to push a file with an already present UUID: ", mPreRequestHeader.mUuid);
            return false;
        }
    }

    if (mPreRequestHeader.mNumHorcruxes > mConfig.mMaxNumHorcruxAccepted)
    {
        Utils::log("The client is requesting to save too many Horcrux for the file. Maximum number of Horcruxes: ", mConfig.mMaxNumHorcruxAccepted);
        return false;
    }
    if (mPreRequestHeader.mTotalSize > mConfig.mMaxTotalSizeAccepted)
    {
        Utils::log("The client is requesting to save a file which is too large. Max size accepted: ", mConfig.mMaxTotalSizeAccepted);
        return false;
    }

    return true;
}

bool TcpConnection::checkRequestHeader() const
{
    if (mRequestHeader.mUuid != mPreRequestHeader.mUuid)
    {
        Utils::log("Error: the client sent an Horcrux with non matching UUID.");
        return false;
    }

    if (mRequestHeader.mIndex >= mCurrentRequestHorcruxes.size())
    {
        Utils::log("Error: the client sent an Horcrux with wrong Index.");
        return false;
    }

    if (mCurrentRequestHorcruxes.at(mRequestHeader.mIndex).has_value())
    {
        Utils::log("Error: the client already sent an Horcrux with the given ID.");
        return false;
    }

    return true;
}

void TcpConnection::ReadPreRequestHeader()
{
    auto self(shared_from_this());
    asio::async_read(mTcpSocket, asio::buffer(mPreRequestHeaderBuffer), [this, self](const asio::error_code& ec, size_t length) {
        if (ec)
        {
            Utils::log("[SERVER] some error occurred: ", ec.message(), " | received message of length: ", length);
            return;
        }
        Utils::log("received N bytes : ", length, " ");
        mPreRequestHeader = Utils::deserializeHeader<Save::PreRequestHeader>(mPreRequestHeaderBuffer);
        Utils::log("Received pre-request header: ", mPreRequestHeader);

        if (checkPreRequestHeader())
        {
            mCurrentRequestHorcruxes.resize(mPreRequestHeader.mNumHorcruxes);
            sendResponse(true, [this, self]() { ReadRequestHeader(); });
        }
        else // return failure and do nothing
        {
            sendResponse(false, [this, self]() {});
        }
    });
}

void TcpConnection::ReadRequestHeader()
{
    auto self(shared_from_this());
    asio::async_read(mTcpSocket, asio::buffer(mRequestHeaderBuffer), [this, self](const asio::error_code& ec, size_t length) {
        if (ec)
        {
            Utils::log("Some error occurred: ", ec.message(), " | received message of length: ", length);
            return;
        }
        Utils::log("Received N bytes : ", length, " ");
        mRequestHeader = Utils::deserializeHeader<Save::Request::Header>(mRequestHeaderBuffer);
        Utils::log("Received horcrux save request header: ", mRequestHeader);

        if (checkRequestHeader())
        {
            mMessageBuffer.resize(mRequestHeader.mContentSize);
            ReadRequestContent();
        }
        else // return failure and do nothing
        {
            sendResponse(false, [this, self]() {});
        }
    });
}

void TcpConnection::ReadRequestContent()
{
    auto self(shared_from_this());
    asio::async_read(mTcpSocket, asio::buffer(mMessageBuffer, mRequestHeader.mContentSize), [this, self](const asio::error_code& ec, size_t length) {
        if (ec)
        {
            Utils::log("Some error occurred: ", ec.message(), " | received message of length: ", length);
            return;
        }

        // store Horcrux Data
        mCurrentRequestHorcruxes[mRequestHeader.mIndex] = HorcruxData{ mMessageBuffer, "" /* ignore checksum */ };

        bool messageComplete = std::all_of(mCurrentRequestHorcruxes.begin(), mCurrentRequestHorcruxes.end(), [](const auto& optHcx) { return optHcx.has_value(); });
        if (messageComplete)
            sendResponse(true, [this, self]() { saveHorcruxes(); }); // end of transmission
        else
            sendResponse(true, [this, self]() { ReadRequestHeader(); }); // keep receiving messages
    });
}

void TcpConnection::writeHandler(const asio::error_code& ec, size_t byteTransferred)
{
    if (!ec)
    {
        Utils::log("Ok, sent ", byteTransferred, " bytes");
        return;
    }
    Utils::log("Something went wrong ", ec.message());
}
} // namespace Horcrux