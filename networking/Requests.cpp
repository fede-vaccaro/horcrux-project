//
// Created by federico on 1/30/24.
//

#include "Requests.hpp"

namespace Horcrux::Save {

std::ostream& operator<<(std::ostream& os, const PreRequestHeader& header)
{
    os << "UUID: " << header.mUuid << ", "
       << "TOTAL_SIZE: " << header.mTotalSize << " NUM_HORCRUXES: " << header.mNumHorcruxes;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Request::Header& header)
{
    os << "UUID: " << header.mUuid << ", "
       << "INDEX: " << header.mIndex << ", "
       << "SIZE: " << header.mContentSize << ", "
       << "CHECKSUM_SIZE: " << header.mChecksumSize;
    return os;
}

PreRequestHeader PreRequestHeader::fromHorcruxes(uint64_t uuid, const std::vector<HorcruxData>& horcruxes)
{
    PreRequestHeader header{ uuid, 0, static_cast<uint32_t>(horcruxes.size()) };

    for (const auto& hcx : horcruxes)
        header.mTotalSize += hcx.mContent.size();

    return header;
}

std::vector<Request> Request::fromHorcruxes(uint64_t uuid, std::vector<HorcruxData>&& horcruxes)
{
    std::vector<Request> requests;
    requests.reserve(horcruxes.size());
    for (auto& hcx : horcruxes)
    {
        Request request;
        request.mHeader = Header{ uuid, static_cast<uint32_t>(requests.size()), static_cast<uint32_t>(hcx.mContent.size()) };
        request.mContent = std::move(hcx.mContent);
        request.mChecksum = std::move(hcx.mChecksum);
        requests.emplace_back(request);
    }
    return requests;
}
} // namespace Horcrux::Save