//
// Created by federico on 1/28/24.
//

#include "Utils.hpp"

#include <random>
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

uint64_t generateRandomUint64()
{
    std::random_device rd;
    std::mt19937_64 rng(rd()); // Mersenne Twister 64-bit generator
    std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());
    return distribution(rng);
}

std::optional<ClientSaveOptions> parseHorcruxCommand(int argc, char* argv[])
{
    ClientSaveOptions opt;

    if (argc != 5)
    {
        std::cerr << "Usage: ./horcrux save -n <horcrux count> <input path>" << std::endl;
        return {};
    }

    std::string command = argv[1];
    if (command != "save")
    {
        std::cerr << "Invalid command. Use 'save' or 'load' ('load' mode is not available)." << std::endl;
        return {};
    }

    std::string option = argv[2];
    if (option != "-n")
    {
        std::cerr << "Invalid option. Use '-n'." << std::endl;
        return {};
    }

    try
    {
        opt.mHorcruxCount = std::stoi(argv[3]);
    } catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid horcrux count. Please provide a valid integer." << std::endl;
        return {};
    }

    opt.mInputPath = argv[4];
    return opt;
}

} // namespace Horcrux::Utils