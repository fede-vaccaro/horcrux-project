//
// Created by federico on 1/28/24.
//

#ifndef HORCRUX_LOGIC_UTILS_HPP_
#define HORCRUX_LOGIC_UTILS_HPP_

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Horcrux::Utils {

template <typename HEADER>
std::array<char, sizeof(HEADER)> serializeHeader(const HEADER& header)
{
    const auto* beginPtr = reinterpret_cast<const char*>(&header);
    const auto* endPtr = beginPtr + sizeof(HEADER);
    std::array<char, sizeof(HEADER)> serialized;
    std::copy(beginPtr, endPtr, serialized.data());
    return serialized;
}

template <typename HEADER>
HEADER deserializeHeader(const std::array<char, sizeof(HEADER)>& binaries)
{
    HEADER header;
    auto* destPtr = reinterpret_cast<char*>(&header);
    std::copy(binaries.begin(), binaries.end(), destPtr);
    return header;
}

template <typename... Args>
void log(const Args&... args)
{
    std::ostringstream oss;
    (oss << ... << args);

    std::cout << "[LOGGER] " << oss.str() << std::endl;
}

// load file from path, returns std::nullopt if something went wrong and logs the error message
std::optional<std::string> loadFile(const std::filesystem::path& path);

bool saveFile(const std::string& binary, const std::filesystem::path& path);

} // namespace Horcrux::Utils
#endif // HORCRUX_LOGIC_UTILS_HPP_
