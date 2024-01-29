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
