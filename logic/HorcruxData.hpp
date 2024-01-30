//
// Created by federico on 1/28/24.
//

#ifndef HORCRUX_LOGIC_HORCRUXDATA_HPP_
#define HORCRUX_LOGIC_HORCRUXDATA_HPP_

#include "Utils.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace Horcrux {

struct HorcruxData
{
    std::string mContent;
    std::string mChecksum;
};

namespace Client {

class ChecksumInterface // (very) stub code
{
public:
    virtual std::string getChecksum(const std::string&) { return ""; }
};

class FileSplitter
{
public:
    virtual std::vector<HorcruxData> split(const std::string& binaryData) = 0;
    virtual std::string join(const std::vector<HorcruxData>& horcrux) = 0;
    virtual ~FileSplitter() = default;
};

class BasicFileSplitter : public FileSplitter
{
public:
    explicit BasicFileSplitter(uint32_t numSplits) : mNumSplits(numSplits) {}

    std::vector<HorcruxData> split(const std::string& binaryData) override;

    std::string join(const std::vector<HorcruxData>& horcrux) override;

private:
    std::unique_ptr<ChecksumInterface> mChecksumMethod = std::make_unique<ChecksumInterface>(); // might be better to inject method from external
    uint32_t mNumSplits;
};

} // namespace Client
}; // namespace Horcrux

#endif // HORCRUX_LOGIC_HORCRUXDATA_HPP_
