//
// Created by federico on 1/28/24.
//

#include "Utils.hpp"
namespace Horcrux::Utils {

std::optional<std::string> loadFile(const std::filesystem::path& path)
{
    std::string out;

    if (!std::filesystem::is_regular_file(path))
    {
        Utils::log("File path does not exist: ", path);
        return {};
    }

    std::ifstream inputFile(path, std::ifstream::binary);
    std::string fileContent;

    if (!inputFile.good())
    {
        Utils::log("Unable to open file :", path);
        return {};
    }

    const auto fileSize = std::filesystem::file_size(path);
    out.resize(fileSize);
    inputFile.read(out.data(), fileSize);
    inputFile.close();
    return out;
}

bool saveFile(const std::string& binary, const std::filesystem::path& path)
{
    if (std::filesystem::is_regular_file(path))
    {
        Utils::log("File already exists: ", path);
        return false;
    }

    std::ofstream outputFile(path, std::ifstream::binary);
    std::string fileContent;

    if (!outputFile.good())
    {
        Utils::log("Unable to write to :", path);
        return false;
    }

    outputFile.write(binary.data(), binary.size());
    return true;
}

} // namespace Horcrux::Utils