#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stdbool.h"
extern"C"
{
const uint8_t BAL_ucMaxMissingAckRetries = 4;
#include "IoConnectionHandling.c"
}

using namespace ::testing;

static struct DBAL_Instance* MainInstancePtr=DBALIN_getMainInstanceForInit();
#ifdef DBAL_CROSS_CONNECTION
static struct DBAL_Instance* UserInstancesPtr=DBALIN_getUserInstancesForInit();
extern "C"
{
extern uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT];
}
#endif

static uint8_t SMM_ExpectedCommState = UINT8_MAX;
static uint8_t SMM_2ndExpectedCommState = UINT8_MAX;
static void SMM_CommStateCallback(enum DBAL_CommState CommState)
{
    EXPECT_EQ(CommState, SMM_ExpectedCommState);
    SMM_ExpectedCommState = UINT8_MAX;
    if(SMM_2ndExpectedCommState != UINT8_MAX)
    {
        SMM_ExpectedCommState = SMM_2ndExpectedCommState;
        SMM_2ndExpectedCommState = UINT8_MAX;
    }
}

static void SMM_MsgLostCallback(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)ServiceId;
    (void)CommandId;
}

#ifdef DBAL_CROSS_CONNECTION
static uint8_t CR_ExpectedCommState = UINT8_MAX;
static uint8_t CR_2ndExpectedCommState = UINT8_MAX;
static void CR_CommStateCallback(enum DBAL_CommState CommState)
{
    EXPECT_EQ(CommState, CR_ExpectedCommState);
    CR_ExpectedCommState = UINT8_MAX;
    if(CR_2ndExpectedCommState != UINT8_MAX)
    {
        CR_ExpectedCommState = CR_2ndExpectedCommState;
        CR_2ndExpectedCommState = UINT8_MAX;
    }
}

static void CR_MsgLostCallback(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    (void)ServiceId;
    (void)CommandId;
}
#endif


class CDBAL_Handler
{
public:
    virtual ~CDBAL_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD1(DBAL_appLayerDBus2Init ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommEnableRequest ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommEnableResponse ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommDisableRequest ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommDisableResponse ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommPingRequest ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommPingResponse ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommTempEnableResponse ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_sendCommTempDisableResponse ,void (struct DBAL_Instance* const Inst));
    MOCK_METHOD0(DBAL_getTargetAddress ,uint8_t (void));
};

class DBALCQ_Handler
{
public:
    virtual ~DBALCQ_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD0(DBALCQ_init, void(void));
};

class DBM_Handler
{
public:
    virtual ~DBM_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD0(DBM_DISABLE_INT, void(void));
    MOCK_METHOD0(DBM_ENABLE_INT, void(void));
};

class DBPL_Handler
{
public:
    virtual ~DBPL_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD0(DBPL_bSendWakeupSentRequest, bool (void));
};


class STIM_Handler
{
public:
    virtual ~STIM_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD4(STDCB_InitCallback, int32_t(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask));
    MOCK_METHOD6(STIM_InitTimer, int32_t(struct STIM_Timer *timer, enum STIM_Processing processingType,
            STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback));
};

class DBAL_IoConnectTest : public Test
{
public:
    static std::unique_ptr<CDBAL_Handler> DBalHandler;
    static std::unique_ptr<DBALCQ_Handler> DBalCqHandler;
    static std::unique_ptr<DBM_Handler> DbmHandler;
    static std::unique_ptr<DBPL_Handler> DbplHandler;
    static std::unique_ptr<STIM_Handler> StimHandler;

    DBAL_IoConnectTest()
    {
        DBalHandler.reset(new ::testing::StrictMock<CDBAL_Handler>());
        DBalCqHandler.reset(new ::testing::StrictMock<DBALCQ_Handler>());
        DbmHandler.reset(new ::testing::StrictMock<DBM_Handler>());
        DbplHandler.reset(new ::testing::StrictMock<DBPL_Handler>());
        StimHandler.reset(new ::testing::StrictMock<STIM_Handler>());
    }

    ~DBAL_IoConnectTest()
    {
        DBalHandler.reset();
        DBalCqHandler.reset();
        DbmHandler.reset();
        DbplHandler.reset();
        StimHandler.reset();
    }

    void expectCall_AppLayerDBus2Init(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_appLayerDBus2Init(Inst)).Times(1);
    }

    void expectCall_EnableRequest(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommEnableRequest(Inst)).Times(1);
    }

    void expectCall_EnableResponse(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommEnableResponse(Inst)).Times(1);
    }

    void expectCall_DisableRequest(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommDisableRequest(Inst)).Times(1);
    }

    void expectCall_DisableResponse(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommDisableResponse(Inst)).Times(1);
    }

    void expectCall_PingRequest(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommPingRequest(Inst)).Times(1);
    }

    void expectCall_PingResponse(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommPingResponse(Inst)).Times(1);
    }

    void expectCall_TempEnableResponse(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommTempEnableResponse(Inst)).Times(1);
    }

    void expectCall_TempDisableResponse(struct DBAL_Instance* const Inst)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_sendCommTempDisableResponse(Inst)).Times(1);
    }

    void expectCall_GetTargetAddress(uint8_t TargetAddress)
    {
        EXPECT_CALL(*this->DBalHandler, DBAL_getTargetAddress()).WillOnce(Return(TargetAddress));
    }

    void expectCall_CrossQueueInit(void)
    {
        EXPECT_CALL(*this->DBalCqHandler, DBALCQ_init()).Times(1);
    }

    void expectCall_DbmDisableInt(void)
    {
        EXPECT_CALL(*this->DbmHandler, DBM_DISABLE_INT()).Times(1);
    }

    void expectCall_DbmEnableInt(void)
    {
        EXPECT_CALL(*this->DbmHandler, DBM_ENABLE_INT()).Times(1);
    }

    void expectCall_DbplWakeupSentRequest(bool RetVal)
    {
        EXPECT_CALL(*this->DbplHandler, DBPL_bSendWakeupSentRequest()).WillOnce(Return(RetVal));
    }

    void expectCall_TimerInitCallback(int32_t ExpectedReturn, struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
    {
        EXPECT_CALL(*this->StimHandler, STDCB_InitCallback(cbPtr, funcPtr, objPtr, eventMask)).WillOnce(Return(ExpectedReturn));
    }

    void expectCall_TimerInit(int32_t ExpectedReturn, struct STIM_Timer *timer, enum STIM_Processing processingType,
            STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback)
    {
        EXPECT_CALL(*this->StimHandler, STIM_InitTimer(timer, processingType, targetTime_ms, timerMode, immediateStart, callback)).WillOnce(Return(ExpectedReturn));
    }

    void expectCall_TimerInitCallback(int32_t ExpectedReturn, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
    {
        EXPECT_CALL(*this->StimHandler, STDCB_InitCallback(_, funcPtr, objPtr, eventMask)).WillOnce(Return(ExpectedReturn));
    }

    void expectCall_TimerInit(int32_t ExpectedReturn, enum STIM_Processing processingType,
            STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart)
    {
        EXPECT_CALL(*this->StimHandler, STIM_InitTimer(_, processingType, targetTime_ms, timerMode, immediateStart, _)).WillOnce(Return(ExpectedReturn));
    }
};

TEST_F(DBAL_IoConnectTest, SMM_CheckFailureBeforeInit)
{
    EXPECT_EQ(DBAL_enable(), false);
    EXPECT_EQ(DBAL_disable(), false);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), false);
}

TEST_F(DBAL_IoConnectTest, SMM_Zero_Init)
{
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    DBAL_init(0x09);/*Invalid address*/
    expectCall_GetTargetAddress(DBAL_PRELIMINARY_PARTNER_ADDR);
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(DBAL_PRELIMINARY_PARTNER_ADDR);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
    EXPECT_EQ(DBAL_registerCommState(nullptr), false);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), false);/*All Slots full.*/
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), true);
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), false);/*All Slots full.*/
    /*Reset and start all over again.*/
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_GetTargetAddress(DBAL_PRELIMINARY_PARTNER_ADDR);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(DBAL_PRELIMINARY_PARTNER_ADDR);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), true);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
}

TEST_F(DBAL_IoConnectTest, SMM_Zero_EnableDisable_OnOurOwn_NoError)
{
    /*Not ready and disconnected, initialized with address zero.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(DBAL_enable(), false);
    EXPECT_EQ(DBAL_disable(), false);
    /*Reset and start all over again.*/
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
}

TEST_F(DBAL_IoConnectTest, SMM_MemRead_Addr_Init)
{
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    DBAL_init(0x09);/*Invalid address*/
    expectCall_GetTargetAddress(0xA1);
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(DBAL_PRELIMINARY_PARTNER_ADDR);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
    EXPECT_EQ(DBAL_registerCommState(nullptr), false);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), false);/*All Slots full.*/
    /*Reset and start all over again.*/
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_GetTargetAddress(0xA1);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(DBAL_PRELIMINARY_PARTNER_ADDR);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
}

TEST_F(DBAL_IoConnectTest, SMM_MemRead_Addr_EnableDisable_OnOurOwn_NoError)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disabled/disconnected*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), false);/*Already connecting.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    MainInstancePtr->SeqId2Send = 123;
    MainInstancePtr->LastSeqIdReceived = 123;
    MainInstancePtr->ConRepeatCnt = 98;
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), false);/*Already connected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disconnecting.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Disable response, also check that status flags for disable handshake are reset.*/
    MainInstancePtr->DisableReqReceived = true;
    MainInstancePtr->ConRepeatCnt = 77;
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(MainInstancePtr->DisableReqReceived, false);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disconnected*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Reset and start all over again.*/
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
}

TEST_F(DBAL_IoConnectTest, SMM_Init)
{
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    DBAL_init(0x09);/*Invalid address*/
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(0xA1);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
    EXPECT_EQ(DBAL_registerCommState(nullptr), false);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), false);/*All Slots full.*/
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), true);
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), false);/*All Slots full.*/
    /*Reset and start all over again.*/
    memset(MainInstancePtr, 0xED, sizeof(struct DBAL_Instance));
    EXPECT_EQ(DBALIN_getMainInstance(), nullptr);
    expectCall_AppLayerDBus2Init(DBALIN_getMainInstanceForInit());
    expectCall_TimerInitCallback(0, DBAL_breakProcessTimerCallback, NULL, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false);
    DBAL_init(0xA1);
    EXPECT_EQ(DBAL_registerCommState(SMM_CommStateCallback), true);
    EXPECT_EQ(DBAL_registerLostMsg(SMM_MsgLostCallback), true);
    EXPECT_EQ(DBALIN_getMainInstance(), MainInstancePtr);
}

TEST_F(DBAL_IoConnectTest, SMM_Disconnected_Receive_Break_No_Enable_Request)
{
    SMM_ExpectedCommState = (uint8_t)DBAL_COMMSTATE_NOT_READY;
    expectCall_DbplWakeupSentRequest(true);
    DBAL_breakSignalDetected();
    /*Confirm state NOT_READY*/
    DBAL_breakProcessTimerCallback(NULL, 0, 0);
}

TEST_F(DBAL_IoConnectTest, SMM_Disconnected_Receive_Break_Enable_Request)
{
    expectCall_DbplWakeupSentRequest(true);
    DBAL_breakSignalDetected();

    /*Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);

    /*Nothing happening...*/
    DBAL_breakProcessTimerCallback(NULL, 0, 0);
}

TEST_F(DBAL_IoConnectTest, SMM_Connected_Receive_Break_No_Enable_Request)
{
    expectCall_DbplWakeupSentRequest(true);
    DBAL_breakSignalDetected();
    /*Nothing happening...*/
    DBAL_breakProcessTimerCallback(NULL, 0, 0);
}

TEST_F(DBAL_IoConnectTest, SMM_Connected_Receive_Break_Enable_Request)
{
    expectCall_DbplWakeupSentRequest(true);
    DBAL_breakSignalDetected();

    /*Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    SMM_2ndExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);

    /*Nothing happening...*/
    DBAL_breakProcessTimerCallback(NULL, 0, 0);

    /*Disable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    expectCall_DisableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_EnableDisable_OnOurOwn_NoError)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disabled/disconnected*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), false);/*Already connecting.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    MainInstancePtr->SeqId2Send = 123;
    MainInstancePtr->LastSeqIdReceived = 123;
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), false);/*Already connected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disconnecting.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Disable response, also check that status flags for disable handshake are reset.*/
    MainInstancePtr->DisableReqReceived = true;
    MainInstancePtr->ConRepeatCnt = 17;
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(MainInstancePtr->DisableReqReceived, false);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Try repeat*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), false);/*Already disconnected*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
}

TEST_F(DBAL_IoConnectTest, SMM_Enable_OnOurOwn_RejectedWhileConnecting)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Reject...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
}

TEST_F(DBAL_IoConnectTest, SMM_Disable_OnOurOwn_RejectedWhileDisConnecting)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    MainInstancePtr->SeqId2Send = 123;
    MainInstancePtr->LastSeqIdReceived = 123;
    MainInstancePtr->ConRepeatCnt = 123;
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Reject...*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_Disable_OnOurOwn_Silent)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    MainInstancePtr->SeqId2Send = 13;
    MainInstancePtr->LastSeqIdReceived = 45;
    MainInstancePtr->ConRepeatCnt = 34;
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Silent..., also check that status flags for disable handshake are reset.*/
    MainInstancePtr->DisableReqReceived = true;
    MainInstancePtr->ConRepeatCnt = 3;
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(MainInstancePtr->DisableReqReceived, false);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_EnableDisable_ByComPartner_NoError)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Disable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    expectCall_DisableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_Enable_ByComPartner_WhileConnecting)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_Enable_ByComPartner_WhileConnected)
{
    /*Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    SMM_2ndExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_EnableDisable_SomeUnexpectedStuff)
{
    /*We are connected from previous test case...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);

    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);

    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);

    /*Disable finally confirmed...*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);

    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);

    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);

    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Some other unexpected action...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);

    /*Abort connection...*/
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);

    /*Enable Request from commpartner, check also that seqId parameters are correctly reset*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    MainInstancePtr->SeqId2Send = 13;
    MainInstancePtr->LastSeqIdReceived = 45;
    MainInstancePtr->ConRepeatCnt = 99;
    expectCall_EnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(MainInstancePtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(MainInstancePtr->ConRepeatCnt, 0);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);

    /*Disable request*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_disable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Disable response*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_EnableDisable_Temporary_NoError)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Temp Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_TempEnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
    /*Temp Disable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    expectCall_TempDisableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_Enable_Temporary_WhileConnecting)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(MainInstancePtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBAL_enable(), true);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Temp Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_TempEnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

TEST_F(DBAL_IoConnectTest, SMM_Enable_Temporary_WhileConnected)
{
    /*Temp Enable Request from commpartner*/
    SMM_ExpectedCommState = DBAL_COMMSTATE_READY;
    expectCall_TempEnableResponse(MainInstancePtr);
    DBAL_connectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    EXPECT_EQ(DBAL_getCommState(), DBAL_COMMSTATE_READY);
    EXPECT_EQ(MainInstancePtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(SMM_ExpectedCommState, UINT8_MAX);
}

#ifdef DBAL_CROSS_CONNECTION
TEST_F(DBAL_IoConnectTest, CR_CheckFailureBeforeInit)
{
    EXPECT_EQ(DBALCR_enable(0U), false);
    EXPECT_EQ(DBALCR_disable(0U), false);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(DBALCR_registerCommState(0U, CR_CommStateCallback), false);

    EXPECT_EQ(DBALCR_enable(1U), false);
    EXPECT_EQ(DBALCR_disable(1U), false);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(DBALCR_registerCommState(1U, CR_CommStateCallback), false);
}

TEST_F(DBAL_IoConnectTest, CR_Init)
{
    DBALCR_ComPartners[0] = 0x09U;/*Invalid Address*/
    EXPECT_EQ(DBALCR_init(), false);
    DBALCR_RolesTowardPartners[0] = DBALCR_PART_TYPE_NONE;/*Not client and not server*/
    expectCall_CrossQueueInit();
    EXPECT_EQ(DBALCR_init(), true); /*Both Address and participant type are invalid -> Ignore and return true.*/
    DBALCR_RolesTowardPartners[0] = DBALCR_PART_TYPE_CLIENT;/*client*/
    DBALCR_ComPartners[0] = 0x23U;/*Valid Address*/
    expectCall_AppLayerDBus2Init(UserInstancesPtr);
    expectCall_TimerInitCallback(0, DBALCR_pingReqTimerCallback, UserInstancesPtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_MIN_PING_TIME_MS, STIM_MODE_SINGLE, false);
    expectCall_AppLayerDBus2Init(UserInstancesPtr+1);
    expectCall_CrossQueueInit();
    EXPECT_EQ(DBALCR_init(), true);
    EXPECT_EQ(DBALCR_registerCommState(0U, CR_CommStateCallback), true);
    EXPECT_EQ(DBALCR_registerCommState(0U, CR_CommStateCallback), false);/*All Slots full.*/
    EXPECT_EQ(DBALCR_registerLostMsg(0U, CR_MsgLostCallback), true);
    EXPECT_EQ(DBALCR_registerLostMsg(0U, CR_MsgLostCallback), false);/*All Slots full.*/
    /*Reset and start all over again.*/
    memset(UserInstancesPtr, 0xED, DBAL_CROSS_CONNECT_COUNT*sizeof(struct DBAL_Instance));
    expectCall_AppLayerDBus2Init(UserInstancesPtr);
    expectCall_TimerInitCallback(0, DBALCR_pingReqTimerCallback, UserInstancesPtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, STIM_PROCESSING_SCHEDULER, DBAL_MIN_PING_TIME_MS, STIM_MODE_SINGLE, false);
    expectCall_AppLayerDBus2Init(UserInstancesPtr+1);
    expectCall_CrossQueueInit();
    EXPECT_EQ(DBALCR_init(), true);
    EXPECT_EQ(DBALCR_registerCommState(2U, CR_CommStateCallback), false);/*Invalid index*/
    EXPECT_EQ(DBALCR_registerCommState(1U, CR_CommStateCallback), true);
    EXPECT_EQ(DBALCR_registerCommState(0U, CR_CommStateCallback), true);
    EXPECT_EQ(DBALCR_registerLostMsg(2U, CR_MsgLostCallback), false);/*Invalid index*/
    EXPECT_EQ(DBALCR_registerLostMsg(1U, CR_MsgLostCallback), true);
    EXPECT_EQ(DBALCR_registerLostMsg(0U, CR_MsgLostCallback), true);
}

TEST_F(DBAL_IoConnectTest, CR_EnableDisable_OnOurOwn_NoError)
{
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_disable(0U), false);/*Already disabled/disconnected*/
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request with invalid index*/
    EXPECT_EQ(DBALCR_enable(2U), false);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(UserInstancesPtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_enable(0U), true);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    UserInstancesPtr->SeqId2Send = 67;
    UserInstancesPtr->LastSeqIdReceived = 89;
    UserInstancesPtr->ConRepeatCnt = 17;
    CR_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(DBALCR_getCommState(2U), DBAL_COMMSTATE_NOT_READY);/*Invalid Index*/
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(UserInstancesPtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    /*Disable request with invalid index*/
    EXPECT_EQ(DBALCR_disable(2U), false);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest(UserInstancesPtr);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_disable(0U), true);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Disable response, also check that status flags for disable handshake are reset.*/
    UserInstancesPtr->DisableReqReceived = true;
    UserInstancesPtr->ConRepeatCnt = 37;
    CR_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(UserInstancesPtr->DisableReqReceived, false);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    EXPECT_EQ(CR_ExpectedCommState, UINT8_MAX);

    /*repeat for the other cross instance, but without provoking errors*/
    /*At beginning, not ready and disconnected.*/
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_disable(1U), false);/*Already disabled/disconnected*/
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest((UserInstancesPtr+1));
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_enable(1U), true);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    /*Enable response, check also that seqId parameters are correctly reset*/
    (UserInstancesPtr+1)->SeqId2Send = 67;
    (UserInstancesPtr+1)->LastSeqIdReceived = 89;
    (UserInstancesPtr+1)->ConRepeatCnt = 77;
    CR_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm((UserInstancesPtr+1), DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(DBALCR_getCommState(2U), DBAL_COMMSTATE_NOT_READY);/*Invalid Index*/
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ((UserInstancesPtr+1)->SeqId2Send, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->ConRepeatCnt, 0);
    /*Disable request*/
    expectCall_DbmDisableInt();
    expectCall_DisableRequest((UserInstancesPtr+1));
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_disable(1U), true);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTING);
    /*Disable response, also check that status flags for disable handshake are reset.*/
    (UserInstancesPtr+1)->DisableReqReceived = true;
    (UserInstancesPtr+1)->ConRepeatCnt = 19;
    CR_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBAL_connectionSm((UserInstancesPtr+1), DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ((UserInstancesPtr+1)->DisableReqReceived, false);
    EXPECT_EQ((UserInstancesPtr+1)->ConRepeatCnt, 0);
    EXPECT_EQ(CR_ExpectedCommState, UINT8_MAX);

}

TEST_F(DBAL_IoConnectTest, CR_EnableDisable_PingTimer)
{
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(UserInstancesPtr);
    expectCall_DbmEnableInt();
    UserInstancesPtr->CrExtension->PingTimeMs = DBAL_MIN_PING_TIME_MS;
    EXPECT_EQ(DBALCR_enable(0U), true);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, false);
    /*Enable response, check also that seqId parameters are correctly reset*/
    UserInstancesPtr->SeqId2Send = 67;
    UserInstancesPtr->LastSeqIdReceived = 89;
    CR_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(DBALCR_getCommState(2U), DBAL_COMMSTATE_NOT_READY);/*Invalid Index*/
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(UserInstancesPtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Call ping timer callback*/
    expectCall_PingRequest(UserInstancesPtr);
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Response or something else->Start timer from the beginning.*/
    DBALCR_triggerPingReqTimerWithStdTime(UserInstancesPtr);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Call ping timer callback*/
    expectCall_PingRequest(UserInstancesPtr);
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Repeat ping*/
    expectCall_PingRequest(UserInstancesPtr);
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Repeat ping last time*/
    expectCall_PingRequest(UserInstancesPtr);
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, true);
    /*Call ping callback last time*/
    CR_ExpectedCommState = DBAL_COMMSTATE_NOT_READY;
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_DISCONNECTED);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, false);
    /*Call ping timer callback after disconnect->no effect*/
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);

    /*Repeat with ping time too small for pinging*/
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest(UserInstancesPtr);
    expectCall_DbmEnableInt();
    UserInstancesPtr->CrExtension->PingTimeMs = DBAL_MIN_PING_TIME_MS - 1U;
    EXPECT_EQ(DBALCR_enable(0U), true);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, false);
    /*Enable response, check also that seqId parameters are correctly reset*/
    UserInstancesPtr->SeqId2Send = 67;
    UserInstancesPtr->LastSeqIdReceived = 89;
    CR_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(0U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(DBALCR_getCommState(2U), DBAL_COMMSTATE_NOT_READY);/*Invalid Index*/
    EXPECT_EQ(UserInstancesPtr->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ(UserInstancesPtr->SeqId2Send, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ(UserInstancesPtr->ConRepeatCnt, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, false);/*Ping timer callback will not be called*/
    /*Call ping timer callback->no effect*/
    DBALCR_pingReqTimerCallback(UserInstancesPtr, 0, 0);
    EXPECT_EQ(UserInstancesPtr->CrExtension->PingReqTimer.enabled, false);

    /*Repeat for the other cross instance*/
    /*Enable request*/
    expectCall_DbmDisableInt();
    expectCall_EnableRequest((UserInstancesPtr+1));
    expectCall_DbmEnableInt();
    (UserInstancesPtr+1)->CrExtension->PingTimeMs = DBAL_MIN_PING_TIME_MS;
    EXPECT_EQ(DBALCR_enable(1U), true);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_NOT_READY);
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTING);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingReqTimer.enabled, false);
    /*Enable response, check also that seqId parameters are correctly reset*/
    UserInstancesPtr->SeqId2Send = 67;
    UserInstancesPtr->LastSeqIdReceived = 89;
    CR_ExpectedCommState = DBAL_COMMSTATE_READY;
    DBAL_connectionSm((UserInstancesPtr+1), DBAL_CON_SM_EVENT_ACCEPT);
    EXPECT_EQ(DBALCR_getCommState(1U), DBAL_COMMSTATE_READY);
    EXPECT_EQ(DBALCR_getCommState(2U), DBAL_COMMSTATE_NOT_READY);/*Invalid Index*/
    EXPECT_EQ((UserInstancesPtr+1)->IoCurrentConnectionState, DBAL_CONNECTIONSTATE_CONNECTED);
    EXPECT_EQ((UserInstancesPtr+1)->SeqId2Send, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->LastSeqIdReceived, UINT8_MAX);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingReqTimer.enabled, false);
    /*Call ping timer callback->no effect*/
    DBALCR_pingReqTimerCallback((UserInstancesPtr+1), 0, 0);
    EXPECT_EQ((UserInstancesPtr+1)->CrExtension->PingReqTimer.enabled, false);
}
#endif

std::unique_ptr<CDBAL_Handler> DBAL_IoConnectTest::DBalHandler;
std::unique_ptr<DBALCQ_Handler> DBAL_IoConnectTest::DBalCqHandler;
std::unique_ptr<DBM_Handler> DBAL_IoConnectTest::DbmHandler;
std::unique_ptr<DBPL_Handler> DBAL_IoConnectTest::DbplHandler;
std::unique_ptr<STIM_Handler> DBAL_IoConnectTest::StimHandler;

void DBAL_appLayerDBus2Init(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_appLayerDBus2Init(Inst);
}

void DBAL_sendCommEnableRequest(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommEnableRequest(Inst);
}

void DBAL_sendCommEnableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommEnableResponse(Inst);
}

void DBAL_sendCommDisableRequest(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommDisableRequest(Inst);
}

void DBAL_sendCommDisableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommDisableResponse(Inst);
}

void DBAL_sendCommPingRequest(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommPingRequest(Inst);
}

void DBAL_sendCommPingResponse(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommPingResponse(Inst);
}

void DBAL_sendCommTempEnableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommTempEnableResponse(Inst);
}

void DBAL_sendCommTempDisableResponse(struct DBAL_Instance* const Inst)
{
    DBAL_IoConnectTest::DBalHandler->DBAL_sendCommTempDisableResponse(Inst);
}

uint8_t DBAL_getTargetAddress(void)
{
    return DBAL_IoConnectTest::DBalHandler->DBAL_getTargetAddress();
}

void DBALCQ_init(void)
{
    DBAL_IoConnectTest::DBalCqHandler->DBALCQ_init();
}

void DBM_DISABLE_INT(void)
{
    DBAL_IoConnectTest::DbmHandler->DBM_DISABLE_INT();
}

void DBM_ENABLE_INT(void)
{
    DBAL_IoConnectTest::DbmHandler->DBM_ENABLE_INT();
}

bool DBPL_bSendWakeupSentRequest(void)
{
    return DBAL_IoConnectTest::DbplHandler->DBPL_bSendWakeupSentRequest();
}

int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
{
    return DBAL_IoConnectTest::StimHandler->STDCB_InitCallback(cbPtr, funcPtr, objPtr, eventMask);
}

int32_t STIM_InitTimer(struct STIM_Timer *timer, enum STIM_Processing processingType,
    STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback)
{
    return DBAL_IoConnectTest::StimHandler->STIM_InitTimer(timer, processingType, targetTime_ms, timerMode, immediateStart, callback);
}
