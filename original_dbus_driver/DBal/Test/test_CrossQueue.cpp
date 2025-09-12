#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stdbool.h"
extern"C"
{
#include "BshDBus2AppLayer_crossQueue.c"
}

using namespace ::testing;

#ifdef DBAL_CROSS_CONNECTION
static struct DBAL_Instance TestInstances[DBAL_CROSS_CONNECT_COUNT];

class IoConnect_Handler
{
public:
    virtual ~IoConnect_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD1(DBAL_getCommStateByInstance, enum DBAL_CommState(const struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_getConnectionStateByInstance, enum DBAL_ConnectionState(const struct DBAL_Instance* const Inst));
};

class DBAL_CrossQueueTest : public Test
{
public:
    static std::unique_ptr<IoConnect_Handler> IoConnectHandler;

    DBAL_CrossQueueTest()
    {
        IoConnectHandler.reset(new ::testing::StrictMock<IoConnect_Handler>());
    }

    ~DBAL_CrossQueueTest()
    {
        IoConnectHandler.reset();
    }

    void expectCall_GetCommState(const struct DBAL_Instance* const Inst, enum DBAL_CommState ExpectedState)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBAL_getCommStateByInstance(Inst)).WillOnce(Return(ExpectedState));
    }

    void expectCall_GetConnectionState(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionState ExpectedState)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBAL_getConnectionStateByInstance(Inst)).WillOnce(Return(ExpectedState));
    }
};

TEST_F(DBAL_CrossQueueTest, ConnectionQueueBeforeInit)
{
    const struct DBAL_Instance* TestPtr = nullptr;
    /*All good*/
    DBALCQ_queueConnectMsg(&TestInstances[0]);
    DBALCQ_queueConnectMsg(&TestInstances[1]);
    EXPECT_EQ(false, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(nullptr, TestPtr);
    TestPtr = nullptr;
}

TEST_F(DBAL_CrossQueueTest, ReqRespQueueBeforeInit)
{
    const struct DBAL_Instance* TestPtr = nullptr;
    uint8_t TestIndex = UINT8_MAX;
    /*All good*/
    TestInstances[0].IoCurrentCommState = DBAL_COMMSTATE_READY;
    TestInstances[1].IoCurrentCommState = DBAL_COMMSTATE_READY;
    TestInstances[0].IoCurrentConnectionState = DBAL_CONNECTIONSTATE_CONNECTED;
    TestInstances[1].IoCurrentConnectionState = DBAL_CONNECTIONSTATE_CONNECTED;
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[1], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[0], 4U);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
}

TEST_F(DBAL_CrossQueueTest, Init)
{
    DBALCQ_init();
}

TEST_F(DBAL_CrossQueueTest, ConnectionQueue)
{
    const struct DBAL_Instance* TestPtr = nullptr;
    DBALCQ_queueConnectMsg(&TestInstances[0]);
    DBALCQ_queueConnectMsg(&TestInstances[1]);
    EXPECT_EQ(true, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(&TestInstances[0], TestPtr);
    TestPtr = nullptr;
    EXPECT_EQ(true, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(&TestInstances[1], TestPtr);
    TestPtr = nullptr;
    EXPECT_EQ(false, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(nullptr, TestPtr);
    TestPtr = nullptr;
    /*Repeat game, try to entry stuff twice, plus swapped order.*/
    DBALCQ_queueConnectMsg(&TestInstances[1]);
    DBALCQ_queueConnectMsg(&TestInstances[1]);
    DBALCQ_queueConnectMsg(&TestInstances[0]);
    DBALCQ_queueConnectMsg(&TestInstances[0]);
    EXPECT_EQ(true, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(&TestInstances[1], TestPtr);
    TestPtr = nullptr;
    DBALCQ_queueConnectMsg(&TestInstances[1]);/*Put to queue again after reading.*/
    DBALCQ_queueConnectMsg(&TestInstances[0]);/*Try to put again, even though not read yet.*/
    EXPECT_EQ(true, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(&TestInstances[0], TestPtr);
    TestPtr = nullptr;
    EXPECT_EQ(true, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(&TestInstances[1], TestPtr);
    TestPtr = nullptr;
    EXPECT_EQ(false, DBALCQ_getConnectMsg(&TestPtr));
    EXPECT_EQ(nullptr, TestPtr);
    TestPtr = nullptr;
}


TEST_F(DBAL_CrossQueueTest, ReqRespQueue)
{
    const struct DBAL_Instance* TestPtr = nullptr;
    uint8_t TestIndex = UINT8_MAX;
    /*CommState not ready->No connection->Reading unsuccessful.*/
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[1], 4U);
    expectCall_GetCommState(&TestInstances[0], DBAL_COMMSTATE_NOT_READY);
    expectCall_GetCommState(&TestInstances[1], DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
    /*CommState ready but still "connecting"*/
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[1], 4U);
    expectCall_GetCommState(&TestInstances[0], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[0], DBAL_CONNECTIONSTATE_CONNECTING);
    expectCall_GetCommState(&TestInstances[1], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[1], DBAL_CONNECTIONSTATE_CONNECTING);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
    /*Connected, but disconnecting/-ed.*/
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[1], 4U);
    expectCall_GetCommState(&TestInstances[0], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[0], DBAL_CONNECTIONSTATE_DISCONNECTING);
    expectCall_GetCommState(&TestInstances[1], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[1], DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
    /*All good*/
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[1], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[0], 4U);
    expectCall_GetCommState(&TestInstances[0], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[0], DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(true, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(&TestInstances[0], TestPtr);
    EXPECT_EQ(4, TestIndex);
    expectCall_GetCommState(&TestInstances[1], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[1], DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(true, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(&TestInstances[1], TestPtr);
    EXPECT_EQ(3, TestIndex);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
    /*Swapped order.*/
    DBALCQ_queueReqRespMsg(&TestInstances[1], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[0], 3U);
    DBALCQ_queueReqRespMsg(&TestInstances[0], 4U);
    expectCall_GetCommState(&TestInstances[1], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[1], DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(true, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(&TestInstances[1], TestPtr);
    EXPECT_EQ(3, TestIndex);
    expectCall_GetCommState(&TestInstances[0], DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(&TestInstances[0], DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(true, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(&TestInstances[0], TestPtr);
    EXPECT_EQ(4, TestIndex);
    EXPECT_EQ(false, DBALCQ_getReqRespMsg(&TestPtr, &TestIndex));
    EXPECT_EQ(nullptr, TestPtr);
    EXPECT_EQ(UINT8_MAX, TestIndex);
}

std::unique_ptr<IoConnect_Handler> DBAL_CrossQueueTest::IoConnectHandler;

enum DBAL_CommState DBAL_getCommStateByInstance(const struct DBAL_Instance* const Inst)
{
    return DBAL_CrossQueueTest::IoConnectHandler->DBAL_getCommStateByInstance(Inst);
}

enum DBAL_ConnectionState DBAL_getConnectionStateByInstance(const struct DBAL_Instance* const Inst)
{
    return DBAL_CrossQueueTest::IoConnectHandler->DBAL_getConnectionStateByInstance(Inst);
}

#endif
