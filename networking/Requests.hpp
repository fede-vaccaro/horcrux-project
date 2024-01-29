//
// Created by federico on 1/28/24.
//

#ifndef HORCRUX_NETWORKING_REQUESTS_HPP_
#define HORCRUX_NETWORKING_REQUESTS_HPP_

#include <cstdint>
#include <string>

namespace Horcrux::Save {

// needed from the server to know what kind of data to expect from the client
struct PreRequestHeader
{
    uint64_t mUuid{};         // file Id
    uint32_t mTotalSize{};    // aggregate size of every horcrux
    uint32_t mNumHorcruxes{}; // index between the total number of Horcruxes

    static PreRequestHeader fromHorcruxes(uint64_t uuid, const std::vector<Horcrux>& horcruxes)
    {
        PreRequestHeader header{ uuid, 0, static_cast<uint32_t>(horcruxes.size()) };

        for (const auto& hcx : horcruxes)
            header.mTotalSize += hcx.mContent.size();

        return header;
    }
};

struct Request
{
    enum class ResponseStatusCode : int32_t
    {
        SUCCESS = 0,
        FAILURE = 1
    };

    struct RequestHeader
    {
        uint64_t mUuid{};         // reference file uuid
        uint32_t mIndex{};        // horcrux index
        uint32_t mContentSize{};  // horcrux size in bytes
        uint32_t mChecksumSize{}; // checksum size in bytes
    };

    // the request header is sent first, to let the server to properly allocate the buffer and to know when to stop reading etc.
    RequestHeader mHeader{};

    // then the actual 'payload' is sent
    std::string mContent{};
    std::string mChecksum{};

    static std::vector<Request> fromHorcruxes(uint64_t uuid, std::vector<Horcrux>&& horcruxes)
    {
        std::vector<Request> requests;
        requests.reserve(horcruxes.size());
        for (auto& hcx : horcruxes)
        {
            Request request;
            request.mHeader = RequestHeader{ uuid, static_cast<uint32_t>(requests.size()), static_cast<uint32_t>(hcx.mContent.size()) };
            request.mContent = std::move(hcx.mContent);
            request.mChecksum = std::move(hcx.mChecksum);
            requests.emplace_back(request);
        }
        return requests;
    }
};

} // namespace Horcrux::Save
#endif // HORCRUX_NETWORKING_REQUESTS_HPP_
