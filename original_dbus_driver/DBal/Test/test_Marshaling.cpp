#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stdbool.h"
#include "utility.h"
extern"C"
{
#include "marshaling.h"
}

using namespace ::testing;

class DBAL_MarshalingTest : public Test
{

};

/**
 * Explanation of naming of test cases:
 * Basic -> Marshal and unmarshal, to make sure, they do the exactly reverse thing.
 * Union -> Unmarshal received data in a way, which would be typical inside the project, to see that endianness really is correct.
 */

TEST_F(DBAL_MarshalingTest, BIGEN_U16_Basic)
{
    const uint16_t TestValue = 0x1234;
    struct BIGEN_U16 Marshaled = {0, 0};

    BIGEN_u16ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hi, 0x12);
    EXPECT_EQ(Marshaled.Lo, 0x34);

    EXPECT_EQ(TestValue, BIGEN_bytesToU16(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, BIGEN_U16_Union)
{
    const uint16_t TestValue = 0x1234;
    uint8_t Data[] = {0x12, 0x34};

    union BIGEN_Union16
    {
        struct BIGEN_U16* StrPtr;
        uint8_t* BytePtr;
    };

    union BIGEN_Union16 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, BIGEN_bytesToU16(TestUnion.StrPtr));
}

TEST_F(DBAL_MarshalingTest, BIGEN_U32_Basic)
{
    const uint32_t TestValue = 0x12345678;
    struct BIGEN_U32 Marshaled = {0, 0, 0, 0};

    BIGEN_u32ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hh, 0x12);
    EXPECT_EQ(Marshaled.Hl, 0x34);
    EXPECT_EQ(Marshaled.Lh, 0x56);
    EXPECT_EQ(Marshaled.Ll, 0x78);

    EXPECT_EQ(TestValue, BIGEN_bytesToU32(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, BIGEN_U32_Union)
{
    const uint32_t TestValue = 0x12345678;
    uint8_t Data[] = {0x12, 0x34, 0x56, 0x78};

    union BIGEN_Union32
    {
        struct BIGEN_U32* StrPtr;
        uint8_t* BytePtr;
    };

    union BIGEN_Union32 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, BIGEN_bytesToU32(TestUnion.StrPtr));
}

TEST_F(DBAL_MarshalingTest, BIGEN_U64_Basic)
{
    const uint64_t TestValue = 0x123456789ABCDEF0;
    struct BIGEN_U64 Marshaled = {0, 0, 0, 0, 0, 0, 0, 0};

    BIGEN_u64ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hhh, 0x12);
    EXPECT_EQ(Marshaled.Hhl, 0x34);
    EXPECT_EQ(Marshaled.Hlh, 0x56);
    EXPECT_EQ(Marshaled.Hll, 0x78);
    EXPECT_EQ(Marshaled.Lhh, 0x9A);
    EXPECT_EQ(Marshaled.Lhl, 0xBC);
    EXPECT_EQ(Marshaled.Llh, 0xDE);
    EXPECT_EQ(Marshaled.Lll, 0xF0);

    EXPECT_EQ(TestValue, BIGEN_bytesToU64(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, BIGEN_U64_Union)
{
    const uint64_t TestValue = 0x123456789ABCDEF0;
    uint8_t Data[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};

    union BIGEN_Union64
    {
        struct BIGEN_U64* StrPtr;
        uint8_t* BytePtr;
    };

    union BIGEN_Union64 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, BIGEN_bytesToU64(TestUnion.StrPtr));
}

TEST_F(DBAL_MarshalingTest, LITEN_U16_Basic)
{
    const uint16_t TestValue = 0x1234;
    struct LITEN_U16 Marshaled = {0, 0};

    LITEN_u16ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hi, 0x12);
    EXPECT_EQ(Marshaled.Lo, 0x34);

    EXPECT_EQ(TestValue, LITEN_bytesToU16(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, LITEN_U16_Union)
{
    const uint16_t TestValue = 0x1234;
    uint8_t Data[] = {0x34, 0x12};

    union LITEN_Union16
    {
        struct LITEN_U16* StrPtr;
        uint8_t* BytePtr;
    };

    union LITEN_Union16 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, LITEN_bytesToU16(TestUnion.StrPtr));
}

TEST_F(DBAL_MarshalingTest, LITEN_U32_Basic)
{
    const uint32_t TestValue = 0x12345678;
    struct LITEN_U32 Marshaled = {0, 0, 0, 0};

    LITEN_u32ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hh, 0x12);
    EXPECT_EQ(Marshaled.Hl, 0x34);
    EXPECT_EQ(Marshaled.Lh, 0x56);
    EXPECT_EQ(Marshaled.Ll, 0x78);

    EXPECT_EQ(TestValue, LITEN_bytesToU32(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, LITEN_U32_Union)
{
    const uint32_t TestValue = 0x12345678;
    uint8_t Data[] = {0x78, 0x56, 0x34, 0x12};

    union LITEN_Union32
    {
        struct LITEN_U32* StrPtr;
        uint8_t* BytePtr;
    };

    union LITEN_Union32 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, LITEN_bytesToU32(TestUnion.StrPtr));
}

TEST_F(DBAL_MarshalingTest, LITEN_U64_Basic)
{
    const uint64_t TestValue = 0x123456789ABCDEF0;
    struct LITEN_U64 Marshaled = {0, 0, 0, 0, 0, 0, 0, 0};

    LITEN_u64ToBytes(&Marshaled, TestValue);//Marshal and check

    EXPECT_EQ(Marshaled.Hhh, 0x12);
    EXPECT_EQ(Marshaled.Hhl, 0x34);
    EXPECT_EQ(Marshaled.Hlh, 0x56);
    EXPECT_EQ(Marshaled.Hll, 0x78);
    EXPECT_EQ(Marshaled.Lhh, 0x9A);
    EXPECT_EQ(Marshaled.Lhl, 0xBC);
    EXPECT_EQ(Marshaled.Llh, 0xDE);
    EXPECT_EQ(Marshaled.Lll, 0xF0);

    EXPECT_EQ(TestValue, LITEN_bytesToU64(&Marshaled));//Unmarshal and check
}

TEST_F(DBAL_MarshalingTest, LITEN_U64_Union)
{
    const uint64_t TestValue = 0x123456789ABCDEF0;
    uint8_t Data[] = {0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12};

    union LITEN_Union64
    {
        struct LITEN_U64* StrPtr;
        uint8_t* BytePtr;
    };

    union LITEN_Union64 TestUnion;
    TestUnion.BytePtr = Data;

    EXPECT_EQ(TestValue, LITEN_bytesToU64(TestUnion.StrPtr));
}
