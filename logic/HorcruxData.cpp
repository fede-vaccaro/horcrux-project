//
// Created by federico on 1/28/24.
//
#include "HorcruxData.hpp"

#include <iterator>
#include <ranges>

namespace Horcrux::Client {
std::vector<HorcruxData> BasicFileSplitter::split(const std::string& binaryData)
{
    if (mNumSplits == 0)
    {
        Utils::log("Chose a number of splits greater than 0");
        return {};
    }

    if (binaryData.size() < mNumSplits)
    {
        Utils::log("Binary data size is smaller than the number of requested splits");
        return {};
    }

    std::vector<HorcruxData> horcruxes;
    horcruxes.reserve(mNumSplits);

    // actual computation
    {
        uint32_t horcruxSize = binaryData.size() / mNumSplits;
        for (int32_t idx = 0; idx < mNumSplits; idx++)
        {
            auto beginIt = binaryData.begin() + idx * horcruxSize;
            auto endIt = idx != mNumSplits - 1 ? binaryData.begin() + (idx + 1) * horcruxSize : binaryData.end();

            std::string horcruxContent(beginIt, endIt);
            std::string checksum = mChecksumMethod->getChecksum(horcruxContent);
            HorcruxData horcrux{ std::move(horcruxContent), std::move(checksum) };
            horcruxes.emplace_back(horcrux);
        }
    }
    return horcruxes;
}
std::string BasicFileSplitter::join(const std::vector<HorcruxData>& horcrux)
{
    std::string joinedHorcrux;

    // get total size to preallocate memory and avoid reallocations
    uint32_t totalByteSize = 0;
    for (const auto& h : horcrux)
        totalByteSize += h.mContent.size();

    joinedHorcrux.reserve(totalByteSize);

    // this is memory inefficient as we use 2X the needed memory
    // we could use incrementally append the horcrux stream
    for (const auto& h : horcrux)
    {
        // assume checksum is ok
        joinedHorcrux += h.mContent;
    }

    return joinedHorcrux;
}

} // namespace Horcrux::Client