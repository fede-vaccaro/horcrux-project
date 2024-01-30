//
// Created by federico on 1/29/24.
//

#include "../logic/HorcruxData.hpp"
#include "../networking/Requests.hpp"

#include <gtest/gtest.h>

namespace Horcrux {

// just for testing
bool operator==(const Save::PreRequestHeader& h1, const Save::PreRequestHeader& h2)
{
    return h1.mUuid == h2.mUuid && h1.mTotalSize == h2.mTotalSize && h1.mNumHorcruxes == h2.mNumHorcruxes;
}

bool operator==(const Save::Request::Header& h1, const Save::Request::Header& h2)
{
    return h1.mUuid == h2.mUuid && h1.mIndex == h2.mIndex && h1.mContentSize == h2.mContentSize;
}

TEST(TestSerialize, TestSerializePreRequestHeader)
{
    Save::PreRequestHeader header{};

    header.mNumHorcruxes = 42;
    header.mTotalSize = 123124;
    header.mUuid = 121212;

    auto serialized = Utils::serializeHeader(header);
    const auto deserialized = Utils::deserializeHeader<Save::PreRequestHeader>(serialized);

    EXPECT_TRUE(header == deserialized);
}

TEST(TestSerialize, TestSerializeRequestHeader)
{
    Save::Request::Header header{};

    header.mUuid = 121212;
    header.mContentSize = 102401;
    header.mIndex = 100;

    auto serialized = Utils::serializeHeader(header);
    const auto deserialized = Utils::deserializeHeader<Save::Request::Header>(serialized);

    EXPECT_TRUE(header == deserialized);
}

TEST(TestRequestSave, TestPreparePreRequestHeaderFromHorcruxes)
{
    std::vector<HorcruxData> inputHcx = { { "AA", "" }, { "BB", "" } };
    const uint64_t uuid = 42;
    Save::PreRequestHeader preRequestHeader = Save::PreRequestHeader::fromHorcruxes(uuid, inputHcx);

    EXPECT_EQ(preRequestHeader.mUuid, uuid);
    EXPECT_EQ(preRequestHeader.mTotalSize, 4);
    EXPECT_EQ(preRequestHeader.mNumHorcruxes, 2);
}

TEST(TestRequestSave, TestPrepareRequestsFromHorcruxes)
{
    std::vector<HorcruxData> inputHcx = { { "AA", "" }, { "BB", "" }, { "CCC", "" } };
    const uint64_t uuid = 42;
    std::vector<Save::Request> requests = Save::Request::fromHorcruxes(uuid, std::move(inputHcx));

    EXPECT_EQ(requests.size(), 3);
    EXPECT_EQ(requests[0].mHeader.mUuid, uuid);
    EXPECT_EQ(requests[0].mHeader.mContentSize, 2);
    EXPECT_EQ(requests[0].mHeader.mChecksumSize, 0);
    EXPECT_EQ(requests[0].mHeader.mIndex, 0);
    EXPECT_EQ(requests[0].mContent, "AA");

    EXPECT_EQ(requests[1].mHeader.mUuid, uuid);
    EXPECT_EQ(requests[1].mHeader.mContentSize, 2);
    EXPECT_EQ(requests[1].mHeader.mChecksumSize, 0);
    EXPECT_EQ(requests[1].mHeader.mIndex, 1);
    EXPECT_EQ(requests[1].mContent, "BB");

    EXPECT_EQ(requests[2].mHeader.mUuid, uuid);
    EXPECT_EQ(requests[2].mHeader.mContentSize, 3);
    EXPECT_EQ(requests[2].mHeader.mChecksumSize, 0);
    EXPECT_EQ(requests[2].mHeader.mIndex, 2);
    EXPECT_EQ(requests[2].mContent, "CCC");
}

} // namespace Horcrux
