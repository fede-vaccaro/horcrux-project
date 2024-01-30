//
// Created by federico on 1/30/24.
//

#include "../logic/Horcrux.hpp"

#include <gtest/gtest.h>

namespace Horcrux::Utils {

class FilesystemUtilsTest : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        if (!exists(kResourceOutDirectory))
            create_directory(kResourceOutDirectory);
    }

    static void TearDownTestSuite()
    {
        if (exists(kResourceOutDirectory))
            remove_all(kResourceOutDirectory);
    }

    inline const static std::filesystem::path kResourceOutDirectory{ "../test/resource_out/" };
    inline const static std::filesystem::path kResourceDirectory{ "../test/resource/" };
};

TEST_F(FilesystemUtilsTest, TestLoadAndSaveFileImage)
{
    // clean-up from previous execution, if any
    const std::filesystem::path outFilePath = kResourceOutDirectory / "harrypotter_copy.png";

    // a bit quick and dirty...
    // ideally should test the methods separately and restrict every test to one case but... i'm very short on time.
    std::string fileBinaries = loadFile(kResourceDirectory / "harrypotter.png").value_or("");
    EXPECT_NE(fileBinaries.size(), 0);

    // i also checked manually if the image is ok :)
    saveFile(fileBinaries, outFilePath);
    EXPECT_TRUE(exists(outFilePath));
    EXPECT_EQ(file_size(kResourceDirectory / "harrypotter.png"), file_size(outFilePath));
}

TEST_F(FilesystemUtilsTest, TestSaveAndLoad)
{
    const std::filesystem::path outFilePath = kResourceOutDirectory / "outfile.test";

    std::string binaries = "AAAABBBBCCCCDDDD";

    // test save
    EXPECT_TRUE(saveFile(binaries, outFilePath));

    // test load
    std::string loadedBinaries = loadFile(outFilePath).value_or("");
    EXPECT_EQ(binaries, loadedBinaries);
}
} // namespace Horcrux::Utils