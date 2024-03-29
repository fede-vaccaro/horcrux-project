#include "../logic/HorcruxData.hpp"

#include <gtest/gtest.h>

const std::filesystem::path resourceDirectory{ "../test/resource/" };

namespace Horcrux {

// Useful for launching all the tests together from the IDE
class BasicFileSplitterTest : public ::testing::Test
{
};

TEST_F(BasicFileSplitterTest, TestZeroSplitsFails)
{
    std::string binaryData = "someData";

    uint32_t nSplits = 0;
    Client::BasicFileSplitter fileSplitter{ nSplits };

    std::vector<HorcruxData> splits = fileSplitter.split(binaryData);

    EXPECT_TRUE(splits.empty());
}

TEST_F(BasicFileSplitterTest, MoreSplitsThanBytesFails)
{
    std::string binaryData = "someData";

    uint32_t nSplits = binaryData.size() + 1;
    Client::BasicFileSplitter fileSplitter{ nSplits };

    std::vector<HorcruxData> splits = fileSplitter.split(binaryData);

    EXPECT_TRUE(splits.empty());
}

TEST_F(BasicFileSplitterTest, TestSplitOk)
{
    std::string binaryData = "AABBCCDD";

    uint32_t nSplits = 4;
    Client::BasicFileSplitter fileSplitter{ nSplits };

    std::vector<HorcruxData> splits = fileSplitter.split(binaryData);

    ASSERT_EQ(splits.size(), nSplits);

    ASSERT_EQ(splits[0].mContent, "AA");
    ASSERT_EQ(splits[1].mContent, "BB");
    ASSERT_EQ(splits[2].mContent, "CC");
    ASSERT_EQ(splits[3].mContent, "DD");
}

TEST_F(BasicFileSplitterTest, TestSplitOkButNumberOfBytesIsNotDivisibleByNumSplits)
{
    std::string binaryData = "AABBCCDD###";

    uint32_t nSplits = 4;
    Client::BasicFileSplitter fileSplitter{ nSplits };

    std::vector<HorcruxData> splits = fileSplitter.split(binaryData);

    ASSERT_EQ(splits.size(), nSplits);

    ASSERT_EQ(splits[0].mContent, "AA");
    ASSERT_EQ(splits[1].mContent, "BB");
    ASSERT_EQ(splits[2].mContent, "CC");
    ASSERT_EQ(splits[3].mContent, "DD###");
}

TEST_F(BasicFileSplitterTest, TestJoin)
{
    const std::string expectedBinaryData = "AABBCCDD";

    std::vector<HorcruxData> input = { { "AA", "" }, { "BB", "" }, { "CC", "" }, { "DD", "" } };
    Client::BasicFileSplitter fileSplitter{ 0 }; // numSplit == input.size() is not checked

    std::string joined = fileSplitter.join(input);
    EXPECT_EQ(joined, expectedBinaryData);
}

} // namespace Horcrux

TEST(HorcruxTestE2E, EndToEndOk)
{
    // load image
    std::string fileBinariesInput = Horcrux::Utils::loadFile(resourceDirectory / "harrypotter.png").value_or("");
    ASSERT_FALSE(fileBinariesInput.empty());

    // split binaries
    uint32_t numSplits = 10;
    Horcrux::Client::BasicFileSplitter splitter(numSplits);
    std::vector<Horcrux::HorcruxData> horcruxes = splitter.split(fileBinariesInput);

    uint32_t hcxCount = 0;
    for (const auto& hcx : horcruxes)
    {
        uint64_t fileUuid = 19203; // some number
        std::filesystem::path destPath = resourceDirectory / (std::to_string(fileUuid) + ".hcx." + std::to_string(hcxCount));
        if (exists(destPath))
            remove(destPath);
        ASSERT_TRUE(Horcrux::Utils::saveFile(hcx.mContent, destPath));
        hcxCount++;
    }

    // reload horcruxes
    std::vector<Horcrux::HorcruxData> reloadedHorcruxes;

    for (uint32_t idx = 0; idx < hcxCount; idx++)
    {
        uint64_t fileUuid = 19203; // some number
        std::filesystem::path destPath = resourceDirectory / (std::to_string(fileUuid) + ".hcx." + std::to_string(idx));
        std::string hcxBinaries = Horcrux::Utils::loadFile(destPath).value_or("");
        ASSERT_FALSE(hcxBinaries.empty());
        reloadedHorcruxes.emplace_back(Horcrux::HorcruxData{ std::move(hcxBinaries), "" });
    }

    std::string rejoinedBinaries = splitter.join(reloadedHorcruxes);
    EXPECT_EQ(rejoinedBinaries, fileBinariesInput);

    // visual test!
    Horcrux::Utils::saveFile(rejoinedBinaries, resourceDirectory / "harrypotter_rejoined.png");
}
