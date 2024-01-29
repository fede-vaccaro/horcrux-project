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
    uint64_t mUuid;            // file Id
    uint16_t mNumHorcruxes;    // index between the total number of Horcruxes
};

struct Request
{
    enum class ResponseStatusCode : uint8_t
    {
        SUCCESS = 0,
        FAILURE = 1
    };

    // it has fixed size
    struct RequestHeader
    {
        uint64_t mUuid;  // file Id
        uint32_t mIndex; // horcrux index
        uint32_t mSize;  // horcrux size in bytes
    };

    RequestHeader mHeader;
    std::string mContent; // content
};

} // namespace Horcrux::Save
#endif // HORCRUX_NETWORKING_REQUESTS_HPP_
