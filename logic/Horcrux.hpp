//
// Created by federico on 1/28/24.
//

#ifndef HORCRUX_LOGIC_HORCRUX_HPP_
#define HORCRUX_LOGIC_HORCRUX_HPP_

#include "Utils.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace Horcrux {

struct Horcrux
{
    std::string mContent;
    std::string mChecksum;
};

namespace Client {

class ChecksumInterface
{
public:
    std::string getChecksum(const std::string& binaries) { return ""; }
};

class FileSplitter
{
public:
    virtual std::vector<Horcrux> split(const std::string& binaryData) = 0;
    virtual std::string join(const std::vector<Horcrux>& horcrux) = 0;
    virtual ~FileSplitter() = default;
};

class BasicFileSplitter : public FileSplitter
{
public:
    explicit BasicFileSplitter(uint32_t numSplits) : mNumSplits(numSplits) {}

    std::vector<Horcrux> split(const std::string& binaryData) override;

    std::string join(const std::vector<Horcrux>& horcrux) override;

private:
    std::unique_ptr<ChecksumInterface> mChecksumMethod = std::make_unique<ChecksumInterface>(); // stub code
    uint32_t mNumSplits;
};

} // namespace Client
}; // namespace Horcrux

#endif // HORCRUX_LOGIC_HORCRUX_HPP_
