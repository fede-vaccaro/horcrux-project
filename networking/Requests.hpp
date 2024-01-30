//
// Created by federico on 1/28/24.
//

#ifndef HORCRUX_NETWORKING_REQUESTS_HPP_
#define HORCRUX_NETWORKING_REQUESTS_HPP_

#include "HorcruxData.hpp"

#include <cstdint>
#include <string>

namespace Horcrux::Save {

// needed from the server to know what kind of data to expect from the client
struct PreRequestHeader
{
    uint64_t mUuid{};         // file Id
    uint32_t mTotalSize{};    // aggregate size of every horcrux
    uint32_t mNumHorcruxes{}; // index between the total number of Horcruxes

    static PreRequestHeader fromHorcruxes(uint64_t uuid, const std::vector<HorcruxData>& horcruxes);
};

std::ostream& operator<<(std::ostream& os, const PreRequestHeader& header);

struct Request
{
    struct Header
    {
        uint64_t mUuid{};         // reference file uuid
        uint32_t mIndex{};        // horcrux index
        uint32_t mContentSize{};  // horcrux size in bytes
        uint32_t mChecksumSize{}; // checksum size in bytes
    };

    // the request header is sent first, to let the server to properly allocate the buffer and to know when to stop reading etc.
    Header mHeader{};

    // then the actual 'payload' is sent
    std::string mContent{};
    std::string mChecksum{};

    static std::vector<Request> fromHorcruxes(uint64_t uuid, std::vector<HorcruxData>&& horcruxes);
};

std::ostream& operator<<(std::ostream& os, const Request::Header& header);

} // namespace Horcrux::Save
#endif // HORCRUX_NETWORKING_REQUESTS_HPP_
