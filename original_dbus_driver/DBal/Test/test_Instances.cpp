#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stdbool.h"
extern"C"
{
#include "BshDBus2AppLayer_instances.c"
}

using namespace ::testing;

#define DBALIN_INIT_PATTERN       0x3243f6A8U // copied from BshDBus2AppLayer_instances.c
static struct DBAL_Instance* MainInstancePtr=nullptr;
static struct DBAL_Instance* UserInstancesPtr=nullptr;
static uint8_t SmmAddr = 0xA1;
static uint8_t CrossAddr= 0x23;
static uint8_t CrossAddr1= 0x71;

class DBAL_InstTest : public Test
{
    public:
    DBAL_InstTest()
    {

    }

    ~DBAL_InstTest()
    {

    }
};


TEST_F(DBAL_InstTest, SetMainInstPtr)
{
    MainInstancePtr = DBALIN_getMainInstanceForInit();
    EXPECT_NE(MainInstancePtr, nullptr);
}

TEST_F(DBAL_InstTest, InitMainInstance)
{
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    DBALIN_init(SmmAddr, nullptr);/*wrong param*/
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    /*Init with correct address of CommPartner*/
    DBALIN_init(SmmAddr, MainInstancePtr);/*right param*/
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
    EXPECT_EQ(MainInstancePtr->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ(MainInstancePtr->CodeSectionBitMask, 0);
    EXPECT_EQ(MainInstancePtr->DBUS_ComPartner, SmmAddr);
    EXPECT_EQ(MainInstancePtr->DBUS_ComBackup, SmmAddr);
    EXPECT_EQ(MainInstancePtr->DisableReqReceived, false);
    EXPECT_EQ(MainInstancePtr->ConnectDataLen, 0);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(MainInstancePtr->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCommStateCbCounter, 0);
    EXPECT_EQ(MainInstancePtr->IoMsgLostCbCounter, 0);
    EXPECT_NE(MainInstancePtr->TxRepeat, nullptr);
    /*Try to reinit with different value of address and see that nothing has changed.*/
    DBALIN_init(0x34, MainInstancePtr);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
    EXPECT_EQ(MainInstancePtr->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ(MainInstancePtr->CodeSectionBitMask, 0);
    EXPECT_EQ(MainInstancePtr->DBUS_ComPartner, SmmAddr);
    EXPECT_EQ(MainInstancePtr->DBUS_ComBackup, SmmAddr);
    EXPECT_EQ(MainInstancePtr->DisableReqReceived, false);
    EXPECT_EQ(MainInstancePtr->ConnectDataLen, 0);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(MainInstancePtr->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCommStateCbCounter, 0);
    EXPECT_EQ(MainInstancePtr->IoMsgLostCbCounter, 0);
    EXPECT_NE(MainInstancePtr->TxRepeat, nullptr);
}

#ifndef DBAL_CROSS_CONNECTION
TEST_F(DBAL_InstTest, SetUserInstPtrNull)
{
    UserInstancesPtr = DBALIN_getUserInstancesForInit();
    EXPECT_EQ(UserInstancesPtr, nullptr);
}

TEST_F(DBAL_InstTest, InitUserInstanceUserInstPtrNull)
{
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), nullptr);
    DBALIN_init(CrossAddr, UserInstancesPtr);
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), nullptr);
}

TEST_F(DBAL_InstTest, GetInstByAddrUserInstPtrNull)
{
    EXPECT_EQ(DBALIN_getInstanceByAddress(SmmAddr), MainInstancePtr);

    for(uint16_t i = 0; i <= UINT8_MAX; i++)
    {
        if((uint8_t)i != SmmAddr)
        {
            EXPECT_EQ(DBALIN_getInstanceByAddress((uint8_t)i), nullptr);
        }
    }
}

TEST_F(DBAL_InstTest, GetUserInstByIndexUserInstPtrNull)
{
    for(uint16_t i = 0; i <= UINT8_MAX; i++)
    {
        EXPECT_EQ(DBALIN_getUserInstanceByIndex((uint8_t)i), nullptr);
    }
}

TEST_F(DBAL_InstTest, GetIndexOfUserInstUserInstPtrNull)
{
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(UserInstancesPtr), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(MainInstancePtr), UINT8_MAX);
}

TEST_F(DBAL_InstTest, UninitAllNullUserInstPtrNull)
{
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));/*uninit*/
    /*Both results, that were not null, are null.*/
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(SmmAddr), nullptr);
    /*Indexes are still invalid.*/
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(UserInstancesPtr), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(MainInstancePtr), UINT8_MAX);
}
#else
TEST_F(DBAL_InstTest, SetUserInstPtrNotNull)
{
    UserInstancesPtr = DBALIN_getUserInstancesForInit();
    EXPECT_NE(UserInstancesPtr, nullptr);
}

TEST_F(DBAL_InstTest, InitUserInstanceUserInstPtrNotNull)
{
    memset(UserInstancesPtr, 0xED, sizeof(struct DBAL_Instance));
    memset((UserInstancesPtr+1), 0xED, sizeof(struct DBAL_Instance));
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), nullptr);
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(1U), nullptr);
    DBALIN_init(CrossAddr, UserInstancesPtr);
    DBALIN_init(CrossAddr1, (UserInstancesPtr+1));

    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), UserInstancesPtr);
    EXPECT_EQ(UserInstancesPtr->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ(UserInstancesPtr->CodeSectionBitMask, 0);
    EXPECT_EQ(UserInstancesPtr->DBUS_ComPartner, CrossAddr);
    EXPECT_EQ(UserInstancesPtr->DBUS_ComBackup, CrossAddr);
    EXPECT_EQ(UserInstancesPtr->DisableReqReceived, false);
    EXPECT_EQ(UserInstancesPtr->ConnectDataLen, 0);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ(UserInstancesPtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(UserInstancesPtr->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCommStateCbCounter, 0);
    EXPECT_EQ(UserInstancesPtr->IoMsgLostCbCounter, 0);
    EXPECT_EQ(UserInstancesPtr->TxRepeat, nullptr);
    EXPECT_EQ(UserInstancesPtr->CrExtension, nullptr);

    EXPECT_EQ(DBALIN_getUserInstanceByIndex(1U), (UserInstancesPtr+1));
    EXPECT_EQ((UserInstancesPtr+1)->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ((UserInstancesPtr+1)->CodeSectionBitMask, 0);
    EXPECT_EQ((UserInstancesPtr+1)->DBUS_ComPartner, CrossAddr1);
    EXPECT_EQ((UserInstancesPtr+1)->DBUS_ComBackup, CrossAddr1);
    EXPECT_EQ((UserInstancesPtr+1)->DisableReqReceived, false);
    EXPECT_EQ((UserInstancesPtr+1)->ConnectDataLen, 0);
    EXPECT_EQ((UserInstancesPtr+1)->ConRepeatCnt, 0);
    EXPECT_EQ((UserInstancesPtr+1)->TransmitDataLen, 0);
    EXPECT_EQ((UserInstancesPtr+1)->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ((UserInstancesPtr+1)->SeqId2Send, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCommStateCbCounter, 0);
    EXPECT_EQ((UserInstancesPtr+1)->IoMsgLostCbCounter, 0);
    EXPECT_EQ((UserInstancesPtr+1)->TxRepeat, nullptr);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension, nullptr);

    /*Try to reinit with different value of address and see that nothing has changed.*/
    DBALIN_init(CrossAddr+3, UserInstancesPtr);
    DBALIN_init(CrossAddr1+3, (UserInstancesPtr+1));

    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), UserInstancesPtr);
    EXPECT_EQ(UserInstancesPtr->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ(UserInstancesPtr->CodeSectionBitMask, 0);
    EXPECT_EQ(UserInstancesPtr->DBUS_ComPartner, CrossAddr);
    EXPECT_EQ(UserInstancesPtr->DBUS_ComBackup, CrossAddr);
    EXPECT_EQ(UserInstancesPtr->DisableReqReceived, false);
    EXPECT_EQ(UserInstancesPtr->ConnectDataLen, 0);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ(UserInstancesPtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(UserInstancesPtr->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCommStateCbCounter, 0);
    EXPECT_EQ(UserInstancesPtr->IoMsgLostCbCounter, 0);
    EXPECT_EQ(UserInstancesPtr->TxRepeat, nullptr);
    EXPECT_EQ(UserInstancesPtr->CrExtension, nullptr);

    EXPECT_EQ(DBALIN_getUserInstanceByIndex(1U), (UserInstancesPtr+1));
    EXPECT_EQ((UserInstancesPtr+1)->InitPattern, DBALIN_INIT_PATTERN);
    EXPECT_EQ((UserInstancesPtr+1)->CodeSectionBitMask, 0);
    EXPECT_EQ((UserInstancesPtr+1)->DBUS_ComPartner, CrossAddr1);
    EXPECT_EQ((UserInstancesPtr+1)->DBUS_ComBackup, CrossAddr1);
    EXPECT_EQ((UserInstancesPtr+1)->DisableReqReceived, false);
    EXPECT_EQ((UserInstancesPtr+1)->ConnectDataLen, 0);
    EXPECT_EQ((UserInstancesPtr+1)->ConRepeatCnt, 0);
    EXPECT_EQ((UserInstancesPtr+1)->TransmitDataLen, 0);
    EXPECT_EQ((UserInstancesPtr+1)->DbalFrames2TransmitCnt, 0);
    EXPECT_EQ((UserInstancesPtr+1)->SeqId2Send, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentCommState, DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCommStateCbCounter, 0);
    EXPECT_EQ((UserInstancesPtr+1)->IoMsgLostCbCounter, 0);
    EXPECT_EQ((UserInstancesPtr+1)->TxRepeat, nullptr);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension, nullptr);
}

TEST_F(DBAL_InstTest, GetInstByAddrUserInstPtrNotNull)
{
    EXPECT_EQ(DBALIN_getInstanceByAddress(SmmAddr), MainInstancePtr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(CrossAddr), UserInstancesPtr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(CrossAddr1), (UserInstancesPtr+1));

    for(uint16_t i = 0; i <= UINT8_MAX; i++)
    {
        if(((uint8_t)i != SmmAddr) && ((uint8_t)i != CrossAddr) && ((uint8_t)i != CrossAddr1))
        {
            EXPECT_EQ(DBALIN_getInstanceByAddress((uint8_t)i), nullptr);
        }
    }
}

TEST_F(DBAL_InstTest, GetUserInstByIndexUserInstPtrNotNull)
{
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(0U), UserInstancesPtr);
    EXPECT_EQ(DBALIN_getUserInstanceByIndex(1U), (UserInstancesPtr+1));

    for(uint16_t i = 2; i < UINT8_MAX; i++)
    {
        EXPECT_EQ(DBALIN_getUserInstanceByIndex((uint8_t)i), nullptr);
    }
}

TEST_F(DBAL_InstTest, GetIndexOfUserInstUserInstPtrNotNull)
{
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(MainInstancePtr), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(UserInstancesPtr), 0U);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+1)), 1U);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+2)), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+3)), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(nullptr), UINT8_MAX);
}

TEST_F(DBAL_InstTest, UninitAllNullUserInstPtrNotNull)
{
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));/*uninit*/
    memset(UserInstancesPtr, 0xED, DBAL_CROSS_CONNECT_COUNT*sizeof(struct DBAL_Instance));/*uninit*/
    /*ALL results, that were not null, are null.*/
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(SmmAddr), nullptr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(CrossAddr), nullptr);
    EXPECT_EQ(DBALIN_getInstanceByAddress(CrossAddr1), nullptr);
    /*Indexes are still the same, because the do not depend on initialization.*/
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(MainInstancePtr), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(UserInstancesPtr), 0U);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+1)), 1U);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+2)), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance((UserInstancesPtr+3)), UINT8_MAX);
    EXPECT_EQ(DBALIN_getIndexOfUserInstance(nullptr), UINT8_MAX);
}

TEST_F(DBAL_InstTest, InitAndQueryCrossPatictipantTypes)
{
    DBALIN_init(CrossAddr, UserInstancesPtr);
    DBALIN_init(CrossAddr1, (UserInstancesPtr+1));
    /*Before initilization*/
    EXPECT_EQ(DBALCR_PART_TYPE_NONE, DBALIN_getCrParticipantTypeByIndex(0));
    EXPECT_EQ(DBALCR_PART_TYPE_NONE, DBALIN_getCrParticipantTypeByIndex(1));
    EXPECT_EQ(DBALCR_PART_TYPE_NONE, DBALIN_getCrParticipantTypeByIndex(2)); /*Out of range*/
    /*Init*/
    DBALIN_initCrExtensionByIndex(0U, DBALCR_PART_TYPE_CLIENT);
    DBALIN_initCrExtensionByIndex(1U, DBALCR_PART_TYPE_SERVER);
    DBALIN_initCrExtensionByIndex(2U, DBALCR_PART_TYPE_SERVER);
    /*After Init*/
    EXPECT_EQ(DBALCR_PART_TYPE_CLIENT, DBALIN_getCrParticipantTypeByIndex(0));
    EXPECT_EQ(DBALCR_PART_TYPE_SERVER, DBALIN_getCrParticipantTypeByIndex(1));
    EXPECT_EQ(DBALCR_PART_TYPE_NONE, DBALIN_getCrParticipantTypeByIndex(2)); /*Out of range*/
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingTimeMs, 0U);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingRepetition, 0U);
    EXPECT_EQ(UserInstancesPtr->CrExtension->OwnPartType, DBALCR_PART_TYPE_CLIENT);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingTimeMs, 0U);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingRepetition, 0U);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->OwnPartType, DBALCR_PART_TYPE_SERVER);
    /*Try to change Init*/
    DBALIN_initCrExtensionByIndex(0U, DBALCR_PART_TYPE_SERVER);
    DBALIN_initCrExtensionByIndex(1U, DBALCR_PART_TYPE_CLIENT);
    DBALIN_initCrExtensionByIndex(2U, DBALCR_PART_TYPE_SERVER);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingTimeMs, 0U);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingRepetition, 0U);
    EXPECT_EQ(UserInstancesPtr->CrExtension->OwnPartType, DBALCR_PART_TYPE_CLIENT);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingTimeMs, 0U);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingRepetition, 0U);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->OwnPartType, DBALCR_PART_TYPE_SERVER);
    /*No change*/
    EXPECT_EQ(DBALCR_PART_TYPE_CLIENT, DBALIN_getCrParticipantTypeByIndex(0));
    EXPECT_EQ(DBALCR_PART_TYPE_SERVER, DBALIN_getCrParticipantTypeByIndex(1));
    EXPECT_EQ(DBALCR_PART_TYPE_NONE, DBALIN_getCrParticipantTypeByIndex(2)); /*Out of range*/
}
#endif
