//
// Created by federico on 1/30/24.
//

#include "HorcruxData.hpp"
#include "Utils.hpp"

using namespace Horcrux;

int main()
{
    auto dirPath = std::filesystem::path{ "out" };
    const uint32_t numHcxs = 10;
    const uint64_t uuid = 1200;

    auto getFileName = [](uint64_t uuid, int32_t index) { return std::to_string(uuid) + ".hcx." + std::to_string(index); };

    std::string binaries;
    for (int32_t idx = 0; idx < numHcxs; idx++)
    {
        auto filename = getFileName(uuid, idx);
        auto loadedBinaries = Utils::loadFile(dirPath / filename).value();
        binaries += loadedBinaries;
    }

    Utils::saveFile(binaries, dirPath / "reconstructed.png");
}