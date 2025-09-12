#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stdbool.h"
#include "utility.h"
extern"C"
{
#include "BshDBus2AppLayer.c"
}

using namespace ::testing;


static struct DBAL_Instance* MainInstancePtr=DBALIN_getMainInstanceForInit();
static const uint8_t SmmAddr = 0xA1;
static const uint8_t PcAddr = 0xC1;
#ifdef DBAL_CROSS_CONNECTION
static struct DBAL_Instance* UserInstancesPtr=DBALIN_getUserInstancesForInit();
static const uint8_t CrossAddr= 0x23;
static const uint8_t CrossAddr1= 0x71;
extern "C"
{
extern uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT];
extern DBALCR_ParticipantType DBALCR_RolesTowardPartners[DBAL_CROSS_CONNECT_COUNT];
}
#endif
/******************************************************************************/
/* DUMMY RECEIVE TABLE SMM                                                    */
/******************************************************************************/
static volatile uint8_t DummyCmdSmm_ExpectedDataLen = UINT8_MAX;
static volatile uint8_t DummyCmdSmm_Bytes[30];
static void dummyCmdSmm(const uint8_t* const Bytes, uint8_t DataLen)
{
    EXPECT_EQ(DataLen, DummyCmdSmm_ExpectedDataLen);
    EXPECT_EQ(0, memcmp((const void*)DummyCmdSmm_Bytes, Bytes, DataLen));
    memset((void*)DummyCmdSmm_Bytes, 0, sizeof(DummyCmdSmm_Bytes));
    DummyCmdSmm_ExpectedDataLen = UINT8_MAX;
}

static volatile uint8_t DummyQuerySmm_ExpectedDataLen = UINT8_MAX;
static volatile uint8_t DummyQuerySmm_Bytes[30];
static void dummyQuerySmm(const uint8_t* const Bytes, uint8_t DataLen)
{
    EXPECT_EQ(DataLen, DummyQuerySmm_ExpectedDataLen);
    EXPECT_EQ(0, memcmp((const void*)DummyQuerySmm_Bytes, Bytes, DataLen));
    memset((void*)DummyQuerySmm_Bytes, 0, sizeof(DummyQuerySmm_Bytes));
    DummyQuerySmm_ExpectedDataLen = UINT8_MAX;
}

const struct DBAL_ReceiveObject DummyReceiveObjectsSmm[] =
{
    {{0x0101, DBAL_TYPE_CMD}, dummyCmdSmm},
    {{0x0102, DBAL_TYPE_QUERY}, dummyQuerySmm}
};

/******************************************************************************/
/* DUMMY RECEIVE TABLE Cross                                                  */
/******************************************************************************/
#ifdef DBAL_CROSS_CONNECTION
static volatile uint8_t DummyCmdRespCr_ExpectedIndex = UINT8_MAX;
static volatile uint8_t DummyCmdRespCr_ExpectedDataLen = UINT8_MAX;
static volatile uint8_t DummyCmdRespCr_Bytes[30];
static void dummyCmdRespCr(uint8_t Index, const uint8_t* const Bytes, uint8_t DataLen)
{
    EXPECT_EQ(Index, DummyCmdRespCr_ExpectedIndex);
    EXPECT_EQ(DataLen, DummyCmdRespCr_ExpectedDataLen);
    EXPECT_EQ(0, memcmp((const void*)DummyCmdRespCr_Bytes, Bytes, DataLen));
    memset((void*)DummyCmdRespCr_Bytes, 0, sizeof(DummyCmdRespCr_Bytes));
    DummyCmdRespCr_ExpectedIndex = UINT8_MAX;
    DummyCmdRespCr_ExpectedDataLen = UINT8_MAX;
}

static volatile uint8_t DummyQueryRespCr_ExpectedIndex = UINT8_MAX;
static volatile uint8_t DummyQueryRespCr_ExpectedDataLen = UINT8_MAX;
static volatile uint8_t DummyQueryRespCr_Bytes[30];
static void dummyQueryRespCr(uint8_t Index, const uint8_t* const Bytes, uint8_t DataLen)
{
    EXPECT_EQ(Index, DummyQueryRespCr_ExpectedIndex);
    EXPECT_EQ(DataLen, DummyQueryRespCr_ExpectedDataLen);
    EXPECT_EQ(0, memcmp((const void*)DummyQueryRespCr_Bytes, Bytes, DataLen));
    memset((void*)DummyQueryRespCr_Bytes, 0, sizeof(DummyQueryRespCr_Bytes));
    DummyQueryRespCr_ExpectedIndex = UINT8_MAX;
    DummyQueryRespCr_ExpectedDataLen = UINT8_MAX;
}

static volatile uint8_t DummyEventCr_ExpectedIndex = UINT8_MAX;
static volatile uint8_t DummyEventCr_ExpectedDataLen = UINT8_MAX;
static volatile uint8_t DummyEventCr_Bytes[30];
static void dummyEventCr(uint8_t Index, const uint8_t* const Bytes, uint8_t DataLen)
{
    EXPECT_EQ(Index, DummyEventCr_ExpectedIndex);
    EXPECT_EQ(DataLen, DummyEventCr_ExpectedDataLen);
    EXPECT_EQ(0, memcmp((const void*)DummyEventCr_Bytes, Bytes, DataLen));
    memset((void*)DummyEventCr_Bytes, 0, sizeof(DummyEventCr_Bytes));
    DummyEventCr_ExpectedIndex = UINT8_MAX;
    DummyEventCr_ExpectedDataLen = UINT8_MAX;
}

const struct DBALCR_ReceiveObject DummyReceiveObjectsCr[] =
{
    {{0x0201, DBAL_TYPE_CMD_ACK}, dummyCmdRespCr},
    {{0x0202, DBAL_TYPE_QUERY_ACK}, dummyQueryRespCr},
    {{0x0203, DBAL_TYPE_EVENT}, dummyEventCr}
};

#endif
/******************************************************************************/
/* DUMMY CALLBACK FOR UNDELIVERED MESSAGES                                    */
/******************************************************************************/

static uint16_t DummyLostServiceIds[DBAL_MAX_MSGS2REPEAT] = {0};
static uint16_t DummyLostCommandIds[DBAL_MAX_MSGS2REPEAT] = {0};
static uint16_t DummyLostDataLen[DBAL_MAX_MSGS2REPEAT] = {0};
static uint8_t DummyLostIndex = 0;
static void dummyCbLostMsg(uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    DummyLostServiceIds[DummyLostIndex] = ServiceId;
    DummyLostCommandIds[DummyLostIndex] = CommandId;
    DummyLostDataLen[DummyLostIndex] = DataLen;
    DummyLostIndex++;
}

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct DBAL_ObjectTableEntry DBAL_ObjectTable[] =
{
    //DBAL_ReceiveObject,                      Module SERVICE_ID,      number of elements in ReceiveObject
    {DummyReceiveObjectsSmm,                   0x1234U,                UTI_NELEMENTS(DummyReceiveObjectsSmm)}
};

// calculation of receive objects count in DBAL_ObjectTable
const uint8_t DBAL_ObjectTableSize = (uint8_t)(sizeof(DBAL_ObjectTable) / sizeof(struct DBAL_ObjectTableEntry));

#ifdef DBAL_CROSS_CONNECTION
// List of Dbus2 addresses of communication partners in ecu to ecu communication.
// Fill with valid values >= 0x10, in the correct order.
// (If the partner in cross connection zero is to have address 0x2F, fill in this value at index zero in the array below.)
uint8_t DBALCR_ComPartners[DBAL_CROSS_CONNECT_COUNT] = {0x23, 0x71};

// List of definitions, whether this controller here is server or client toward communication partners.
// Enter info at same index as in DBALCR_ComPartners.
enum DBALCR_ParticipantType DBALCR_RolesTowardPartners[DBAL_CROSS_CONNECT_COUNT] = {DBALCR_PART_TYPE_CLIENT, DBALCR_PART_TYPE_SERVER};

const struct DBALCR_ObjectTableEntry DBALCR_ObjectTable[]=
{
    //DBALCR_ReceiveObject,                      Module SERVICE_ID,      number of elements in ReceiveObject
    {DummyReceiveObjectsCr,                      0x1234U,                UTI_NELEMENTS(DummyReceiveObjectsCr)}
};
const uint8_t DBALCR_ObjectTableSize = (uint8_t)(sizeof(DBALCR_ObjectTable) / sizeof(struct DBALCR_ObjectTableEntry));
#endif
/******************************************************************************/
/* OPERATORS AND HELPERS                                                      */
/******************************************************************************/

/******************************************************************************/
/* CLASSES                                                                    */
/******************************************************************************/
class BAL_Handler
{
public:
    virtual ~BAL_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD2(BAL_vTransmitMessage, void(uint8_t ucSubsystem, uint8_t ucMessageNumber));
    MOCK_METHOD2(BAL_vCancelTransmitMessage, void(uint8_t ucSubsystem, uint8_t ucMessageNumber));
    MOCK_METHOD0(BAL_vSendWakeupBreak, void(void));
};

class DBPL_Handler
{
public:
    virtual ~DBPL_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD0(DBPL_vUnNotifyWakeupBreakSent, void(void));
};

class DBM_Handler
{
public:
    virtual ~DBM_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD0(DBM_DISABLE_INT, void(void));
    MOCK_METHOD0(DBM_ENABLE_INT, void(void));
};

class STIM_Handler
{
public:
    virtual ~STIM_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD4(STDCB_InitCallback, int32_t(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask));
    MOCK_METHOD6(STIM_InitTimer, int32_t(struct STIM_Timer *timer, enum STIM_Processing processingType,
            STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback));
};

class IoConnect_Handler
{
public:
    virtual ~IoConnect_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD1(DBAL_getCommStateByInstance, enum DBAL_CommState(const struct DBAL_Instance* const Inst));
    MOCK_METHOD1(DBAL_getConnectionStateByInstance, enum DBAL_ConnectionState(const struct DBAL_Instance* const Inst));
    MOCK_METHOD2(DBAL_connectionSm, void(struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Action));
    MOCK_METHOD1(DBALCR_triggerPingReqTimerWithStdTime, void(const struct DBAL_Instance* const Inst));
};

class DbalNtf_Handler
{
public:
    virtual ~DbalNtf_Handler() {} // IMPORTANT: This is needed by std::unique_ptr
    MOCK_METHOD6(DBAL_ntfUnknownDBalFrameReceived, void(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen));
    MOCK_METHOD6(DBAL_ntfUnexpectedDBalFrameReceived, void(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen));
    MOCK_METHOD2(DBAL_ntfCorruptReqRespDbus2FrameReceived, void(const uint8_t* const Bytes, uint8_t DataLen));
    MOCK_METHOD2(DBAL_ntfCorruptConDbus2FrameReceived, void(const uint8_t* const Bytes, uint8_t DataLen));
    MOCK_METHOD1(DBAL_storeTargetAddress, void(uint8_t TargetAddress));
};

class DBAL_AppLayerTest : public Test
{
public:
    static std::unique_ptr<BAL_Handler> BalHandler;
    static std::unique_ptr<DBPL_Handler> DbplHandler;
    static std::unique_ptr<DBM_Handler> DbmHandler;
    static std::unique_ptr<STIM_Handler> StimHandler;
    static std::unique_ptr<IoConnect_Handler> IoConnectHandler;
    static std::unique_ptr<DbalNtf_Handler> DbalNtfHandler;

    DBAL_AppLayerTest()
    {
        BalHandler.reset(new ::testing::StrictMock<BAL_Handler>());
        DbplHandler.reset(new ::testing::StrictMock<DBPL_Handler>());
        DbmHandler.reset(new ::testing::StrictMock<DBM_Handler>());
        StimHandler.reset(new ::testing::StrictMock<STIM_Handler>());
        IoConnectHandler.reset(new ::testing::StrictMock<IoConnect_Handler>());
        DbalNtfHandler.reset(new ::testing::StrictMock<DbalNtf_Handler>());
    }

    ~DBAL_AppLayerTest()
    {
        BalHandler.reset();
        DbplHandler.reset();
        DbmHandler.reset();
        StimHandler.reset();
        IoConnectHandler.reset();
        DbalNtfHandler.reset();
    }

    void expectCall_BalTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->BalHandler, BAL_vTransmitMessage(ucSubsystem, ucMessageNumber)).Times(Times);
    }

    void expectCall_BalCancelTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->BalHandler, BAL_vCancelTransmitMessage(ucSubsystem, ucMessageNumber)).Times(Times);
    }

    void expectCall_BalSendWakeup(uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->BalHandler, BAL_vSendWakeupBreak()).Times(Times);
    }

    void expectCall_DbplUnNotifyWakeup(uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbplHandler, DBPL_vUnNotifyWakeupBreakSent()).Times(Times);
    }

    void expectCall_DbmDisableInt(uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbmHandler, DBM_DISABLE_INT()).Times(Times);
    }

    void expectCall_DbmEnableInt(uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbmHandler, DBM_ENABLE_INT()).Times(Times);
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

    void expectCall_GetCommState(const struct DBAL_Instance* const Inst, enum DBAL_CommState ExpectedState, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBAL_getCommStateByInstance(Inst)).Times(Times).WillRepeatedly(Return(ExpectedState));
    }

    void expectCall_GetConnectionState(const struct DBAL_Instance* const Inst, enum DBAL_ConnectionState ExpectedState, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBAL_getConnectionStateByInstance(Inst)).Times(Times).WillRepeatedly(Return(ExpectedState));
    }

    void expectCall_ConnectionSm(struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Action, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBAL_connectionSm(Inst, Action)).Times(Times);
    }

    void expectCall_TriggerPingReqTimer(const struct DBAL_Instance* const Inst, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->IoConnectHandler, DBALCR_triggerPingReqTimerWithStdTime(Inst)).Times(Times);
    }

    void expectCall_UnknownDBalFrame(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbalNtfHandler, DBAL_ntfUnknownDBalFrameReceived(SenderNodeAddress, DBalType, ServiceId, CommandId, Bytes, DataLen)).Times(Times);
    }

    void expectCall_UnexpectedDBalFrame(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbalNtfHandler, DBAL_ntfUnexpectedDBalFrameReceived(SenderNodeAddress, DBalType, ServiceId, CommandId, Bytes, DataLen)).Times(Times);
    }

    void expectCall_CorruptReqRespDbus2Frame(const uint8_t* const Bytes, uint8_t DataLen, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbalNtfHandler, DBAL_ntfCorruptReqRespDbus2FrameReceived(Bytes, DataLen)).Times(Times);
    }

    void expectCall_CorruptConDbus2Frame(const uint8_t* const Bytes, uint8_t DataLen, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbalNtfHandler, DBAL_ntfCorruptConDbus2FrameReceived(Bytes, DataLen)).Times(Times);
    }

    void expectCall_StoreTargetAddress(uint8_t TargetAddress, uint8_t Times = 1U)
    {
        EXPECT_CALL(*this->DbalNtfHandler, DBAL_storeTargetAddress(TargetAddress)).Times(Times);
    }
};
/******************************************************************************/
/* TEST CASES                                                                 */
/******************************************************************************/
TEST_F(DBAL_AppLayerTest, SMM_CheckFailureBeforeInit)
{
    const uint8_t Arr1[] = {1, 2, 3, 4};
    const uint8_t Arr2[] = {5, 6, 7};
    const uint8_t Arr3[] = {8, 9};
    EXPECT_EQ(DBAL_sendCmdResponse(1, 2, Arr1, (uint8_t)sizeof(Arr1)), false);
    EXPECT_EQ(DBAL_sendQueryResponse(3, 4, Arr2, (uint8_t)sizeof(Arr2)), false);
    EXPECT_EQ(DBAL_sendEvent(5, 6, Arr3, (uint8_t)sizeof(Arr3)), false);
}

TEST_F(DBAL_AppLayerTest, SMM_Init)
{
    uint8_t TargetAddress = SmmAddr;
    DBALIN_init(TargetAddress, MainInstancePtr);

    expectCall_TimerInitCallback(0, &MainInstancePtr->ConMsgCbInst, DBAL_conMsgTimerCallback, MainInstancePtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &MainInstancePtr->ConMsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false, &MainInstancePtr->ConMsgCbInst);
    expectCall_TimerInitCallback(0, &MainInstancePtr->MsgCbInst, DBAL_msgTimerCallback, MainInstancePtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &MainInstancePtr->MsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_RESPONSE_TIME_MS, STIM_MODE_SINGLE, false, &MainInstancePtr->MsgCbInst);

    DBAL_appLayerDBus2Init(MainInstancePtr);

    MainInstancePtr->IoMsgLostCbArray[0] = dummyCbLostMsg;
    MainInstancePtr->IoMsgLostCbCounter++;
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionSend_NoError)
{
    /*At start transmit buffer empty.*/
    for(uint8_t i = 0; i < DBAL_CON_MSG_LEN; i++)
    {
        EXPECT_EQ(MainInstancePtr->ConnectTransmitBuffer[i], 0U);
    }

    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*ENABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*DISABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer2[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer2, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer2[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer2);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer2, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*DISABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer3[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer3, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer3[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer3);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer3, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*PING RESPONSE (disconnecting)*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(MainInstancePtr, DBAL_CONNECTIONSTATE_DISCONNECTING);
    DBAL_sendCommPingResponse(MainInstancePtr);

    /*PING RESPONSE (disconnected)*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_NOT_READY);
    DBAL_sendCommPingResponse(MainInstancePtr);

    /*PING RESPONSE (connected)*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(MainInstancePtr, DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommPingResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer4[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer4, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer4[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer4);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer4, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*TEMP ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommTempEnableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer5[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_TEMP_ENABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer5, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer5[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer5);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer5, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*TEMP DISABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommTempDisableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer6[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_TEMP_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer6, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer6[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer6);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer6, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionSend_EnableAfterDisable)
{
    /*DISABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));

    /*ENABLE REQUEST before confirmation function*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));

    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();

    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionSend_DisableOverriddenByEnable)
{
    /*DISABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*ENABLE REQUEST before picking up data*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionEnableReqSend_ErrorDisable)
{
    /*Covered errors are: DLL_ACK_WRONG and DLL_ACK_BUSY*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*3. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*4. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*5. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*6. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionDisableReqSend_ErrorDisable)
{
    /*No Dbus2 Error, but response timeout...*/
    MainInstancePtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*3. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*4. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*5. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*6. Delivery ended*/
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    MainInstancePtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionEnableReqSend_ErrorHappyEnd)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*2. Sending attempt is triggered from timer.*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_conMsgTimerCallback(MainInstancePtr, 0U, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*3. Sending attempt is triggered from timer.*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_conMsgTimerCallback(MainInstancePtr, 0U, 0);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionEnableReqSend_WakeupSentRequest)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*2. Sending attempt by WakeupSentRequest.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    EXPECT_FALSE(DBAL_isNodeToBeWokenUp(SmmAddr));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    /*Another WakeupSentRequest without effect*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_FALSE(DBAL_isNodeToBeWokenUp(SmmAddr));
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionDisableReqSend_ErrorHappyEnd)
{
    /*No Dbus2 Error, but response timeout...*/
    MainInstancePtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommDisableRequest(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)MainInstancePtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery ended*/
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
    /*Receive Response*/
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {SmmAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    MainInstancePtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionEnableRespSend_ErrorNoAck)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommEnableResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_REJECT);
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionPingRespSend_ErrorNoAck)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*PING RESPONSE*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(MainInstancePtr, DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_sendCommPingResponse(MainInstancePtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionReceive)
{
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {SmmAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};

    /*Set stuff, that can be reset by received message.*/
    MainInstancePtr->DbalFrames2TransmitCnt = 0xED;
    MainInstancePtr->TransmitDataLen = 0xEDU;
    memset(MainInstancePtr->TransmitBuffer, 0xEDU, sizeof(MainInstancePtr->TransmitBuffer));
    MainInstancePtr->DBUS_ComPartner=SmmAddr;
    MainInstancePtr->DBUS_ComBackup=SmmAddr;
    /*ComPartner is unknown.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = 0x57;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN-1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN-1U, ReceiveBuffer);
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN+1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN+1U, ReceiveBuffer);
    /*Wrong protocol type*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = 0x02;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Wrong con msg type*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_MSG_TYPE_COUNT;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_REQUEST;
    /*Wrong protocol version*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION+1;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION;
    /*Check, that params set at start are unchanged.*/
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0xEDU);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0xEDU);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0xEDU);
    }

    /*Correct Enable Request*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }

    /*Set stuff, that can be reset by received message.*/
    MainInstancePtr->DbalFrames2TransmitCnt = 0xED;
    MainInstancePtr->TransmitDataLen = 0xEDU;
    memset(MainInstancePtr->TransmitBuffer, 0xEDU, sizeof(MainInstancePtr->TransmitBuffer));

    /*Correct Enable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }

    /*Correct Disable Request*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Disable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Ping Request (disconnected)*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_NOT_READY);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Ping Request (connected)*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(MainInstancePtr, DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    /*Ping Response is sent.*/
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_ConnectionReceive_Addr_Zero)
{
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {SmmAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};

    /*Set stuff, that can be reset by received message.*/
    MainInstancePtr->DbalFrames2TransmitCnt = 0xED;
    MainInstancePtr->TransmitDataLen = 0xEDU;
    memset(MainInstancePtr->TransmitBuffer, 0xEDU, sizeof(MainInstancePtr->TransmitBuffer));
    MainInstancePtr->DBUS_ComPartner=DBAL_PRELIMINARY_PARTNER_ADDR;
    MainInstancePtr->DBUS_ComBackup=DBAL_PRELIMINARY_PARTNER_ADDR;
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN-1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN-1U, ReceiveBuffer);
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN+1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN+1U, ReceiveBuffer);
    /*Wrong protocol type*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = 0x02;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Wrong con msg type*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_MSG_TYPE_COUNT;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_REQUEST;
    /*Wrong protocol version*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION+1;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION;
    /*Check, that params set at start are unchanged.*/
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0xEDU);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0xEDU);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0xEDU);
    }

    /*Correct Enable Request*/
    expectCall_StoreTargetAddress(SmmAddr);
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }

    /*Set stuff, that can be reset by received message.*/
    MainInstancePtr->DbalFrames2TransmitCnt = 0xED;
    MainInstancePtr->TransmitDataLen = 0xEDU;
    memset(MainInstancePtr->TransmitBuffer, 0xEDU, sizeof(MainInstancePtr->TransmitBuffer));

    /*Correct Enable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }

    /*Correct Disable Request*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Disable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    /*Reset Stuff, which is done by IoConnectionHandling*/
    MainInstancePtr->DisableReqReceived = false;

    /*Correct Ping Request (disconnected)*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_NOT_READY);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Ping Request (connected)*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState(MainInstancePtr, DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CONNECTION);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    /*Ping Response is sent.*/
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmit_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, SMM_TempConnectionReceive)
{
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {SmmAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    MainInstancePtr->DBUS_ComPartner=SmmAddr;
    MainInstancePtr->DBUS_ComBackup=SmmAddr;

    /*Temp Enable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Request -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Enable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Repeat Temp Enable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Enable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Enable Request -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_ENABLE_REQUEST;
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Enable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_ENABLE_RESPONSE;
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Disable Request -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_DISABLE_REQUEST;
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Disable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_DISABLE_RESPONSE;
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    MainInstancePtr->DBUS_ComPartner=SmmAddr;/*Manual reset.*/

    /*Repeat Temp Disable Request -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Enable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = SmmAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_ENABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Enable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Response -> No effect.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Enable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_ENABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Temp Disable Request*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = PcAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = (uint8_t)DBAL_CON_TEMP_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_TEMP_DISABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    MainInstancePtr->DBUS_ComPartner=SmmAddr;/*Manual reset.*/
}

TEST_F(DBAL_AppLayerTest, SMM_Send_NoError)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint8_t Dbus2SendBuffer[50];
    /*At start transmit buffer empty.*/
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }

    /*Send query response*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendQueryResponse(0x1234U, 0x5678U, Data, 10U));
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY_ACK, 10, 0x12, 0x34, 0x56, 0x78, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 19));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 19);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(19, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 19));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 19));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Send command response and event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, 2U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));/*Duplicate: Same ServiceId, CommandId and payload.*/

    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_CMD_ACK, 2, 0x9A, 0xBC, 0xDE, 0xF0, 1, 2,
                                                                                       DBAL_TYPE_EVENT,   3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer1, 20));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 20);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 2);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(20, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 20));
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, Dbus2SendBuffer, 20));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm event*/
    uint8_t EventConfirm[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x11, 0x22, 0x33, 0x44};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm), EventConfirm);
    /*Call timer callback. No mocks are called, so event has not been repeated.*/
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, SMM_Send_EventRepeat)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send command response and event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, 2U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 2, 0x9A, 0xBC, 0xDE, 0xF0, 1, 2,
                                                                                       DBAL_TYPE_EVENT,   3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 20));
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 2);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(20, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 20));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 20));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer1, 12));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, Dbus2SendBuffer, 12));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm event, but with payload, which is wrong*/
    uint8_t EventConfirm0[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 1, 0x11, 0x22, 0x33, 0x44, 0xFF};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_UnexpectedDBalFrame(SmmAddr, DBAL_TYPE_EVENT_ACK, 0x1122, 0x3344, &EventConfirm0[9], 1U);
    DbalReceive_appLayerResponse(sizeof(EventConfirm0), EventConfirm0);

    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    const uint8_t ExpectedBuffer2[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer2, 12));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(ExpectedBuffer2, Dbus2SendBuffer, 12));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm event*/
    uint8_t EventConfirm[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_EVENT_ACK, 0, 0x11, 0x22, 0x33, 0x44};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm), EventConfirm);
    /*Call timer callback. No mocks are called, so event has not been repeated.*/
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, SMM_Send_EventsRepeatFIFO)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send three events*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x9ABCU, 0xDEF0U, Data, 2U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x5566U, 0x7788U, Data, 1U));

    /*Call timer callback, to trigger repetition.*/
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 2, 0x9A, 0xBC, 0xDE, 0xF0, 1, 2,
                                                                                       DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3,
                                                                                       DBAL_TYPE_EVENT, 1, 0x55, 0x66, 0x77, 0x88, 1};

    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 27));
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 3);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(27, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 27));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 27));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm middle event*/
    uint8_t EventConfirm0[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x11, 0x22, 0x33, 0x44};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm0), EventConfirm0);
    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY, 2);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    /*Add new event.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x99AAU, 0xBBCCU, Data, 4U));

    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 2, 0x9A, 0xBC, 0xDE, 0xF0, 1, 2,
                                                                                       DBAL_TYPE_EVENT, 1, 0x55, 0x66, 0x77, 0x88, 1,
                                                                                       DBAL_TYPE_EVENT, 4, 0x99, 0xAA, 0xBB, 0xCC, 1, 2, 3, 4};

    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer1, 28));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 28);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 3);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(28, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 28));
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, Dbus2SendBuffer, 28));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm remaining events.*/
    uint8_t EventConfirm1[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_EVENT_ACK, 0, 0x9A, 0xBC, 0xDE, 0xF0,
                                                                 DBAL_TYPE_EVENT_ACK, 0, 0x55, 0x66, 0x77, 0x88,
                                                                 DBAL_TYPE_EVENT_ACK, 0, 0x99, 0xAA, 0xBB, 0xCC};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm1), EventConfirm1);

    /*Call timer callback. No mocks are called, so event has not been repeated.*/
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, SMM_Send_EventNotRespondedAtAll)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send command response and event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, 2U));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 2, 0x9A, 0xBC, 0xDE, 0xF0, 1, 2,
                                                                                       DBAL_TYPE_EVENT,   3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 20));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 20);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 2);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(20, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 20));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 20));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    for(uint8_t k = 0; k < (DBAL_DBUS_RETRY_MAX - 1); k++)/*all repetitions, but one*/
    {
        /*Call timer callback, to trigger repetition.*/
        expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
        expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
        DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
        const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0U, DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
        EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer1, 12));
        EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
        EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
        /*Data are picked up by dbus*/
        expectCall_DbmDisableInt();
        expectCall_DbmEnableInt();
        DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
        EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
        EXPECT_EQ(0, memcmp(ExpectedBuffer1, Dbus2SendBuffer, 12));
        /*Clear buffer by confirmation function.*/
        DbalTransmitPost_appLayerResponse();
        for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
        {
            EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
            EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
        }
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
        EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
        EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
        (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));
    }
    /*last repetition*/
    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0U, DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer1, 12));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, Dbus2SendBuffer, 12));
    /*try to add one more event; we still should be on last timer repetition*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x5566U, 0x7788U, Data, 0U));
    /*Clear buffer by confirmation function.*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DbalTransmitPost_appLayerResponse();

    /*At first, confirmation function triggers sending of new event.*/
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 9);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Call timer callback once again, connection becomes silent.*/
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    DBAL_clearBothIoTxBuffers(MainInstancePtr);/*Do, what reconnecting would do.*/
    /*Check notification about undelivered message.*/
    EXPECT_EQ(DummyLostServiceIds[0], 0x1122U);
    EXPECT_EQ(DummyLostCommandIds[0], 0x3344U);
    EXPECT_EQ(DummyLostDataLen[0], 3);
    EXPECT_EQ(DummyLostServiceIds[1], 0x5566U);
    EXPECT_EQ(DummyLostCommandIds[1], 0x7788U);
    EXPECT_EQ(DummyLostDataLen[1], 0);
    /*Reset Values*/
    DummyLostServiceIds[0] = 0;
    DummyLostCommandIds[0] = 0;
    DummyLostDataLen[0] = 0;
    DummyLostServiceIds[1] = 0;
    DummyLostCommandIds[1] = 0;
    DummyLostDataLen[1] = 0;
    DummyLostIndex = 0;
}

TEST_F(DBAL_AppLayerTest, SMM_Send_BufferOverflow)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Try to send event alone. -> Buffer of payload in repetition queue too small.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, sizeof(Data)));/*Repetition buffer too small*/
    /*Send command response and event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, sizeof(Data)));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, sizeof(Data)));/*Repetition buffer too small*/

    /*Event not fitting into buffer.*/
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_CMD_ACK, 20, 0x9A, 0xBC, 0xDE, 0xF0,
            1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 29));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 29);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 29));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 29));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));
}

TEST_F(DBAL_AppLayerTest, SMM_Send_EventQueueOverflow)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage, data too long to fit in queue for repetition.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, sizeof(Data)));
    /*Send event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 2));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data+2, 2));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x5566U, 0x7788U, Data+4, 2));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x5566U, 0x7788U, Data+6, 2));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage, does not fit into queue for repetition.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x99AAU, 0xBBCCU, Data+8, 2));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1,
            DBAL_TYPE_EVENT, 2, 0x11, 0x22, 0x33, 0x44, 1, 2,
            DBAL_TYPE_EVENT, 2, 0x11, 0x22, 0x33, 0x44, 3, 4,
            DBAL_TYPE_EVENT, 2, 0x55, 0x66, 0x77, 0x88, 5, 6,
            DBAL_TYPE_EVENT, 2, 0x55, 0x66, 0x77, 0x88, 7, 8};

    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 35));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 35);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 4);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(35, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 35));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 35));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm events*/
    uint8_t EventConfirm[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x11, 0x22, 0x33, 0x44,
                                                                DBAL_TYPE_EVENT_ACK, 0, 0x55, 0x66, 0x77, 0x88};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm), EventConfirm);
    /*Call timer callback. No mocks are called, so events have not been repeated.*/
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, SMM_Send_EventsNoAck)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage, data too long to fit in queue for repetition.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, sizeof(Data)));
    /*Send event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 2));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x5566U, 0x7788U, Data+2, 3));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x99AAU, 0xBBCCU, Data+5, 4));
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0xDDEEU, 0xFF00U, Data+9, 5));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1,
            DBAL_TYPE_EVENT, 2, 0x11, 0x22, 0x33, 0x44, 1, 2,
            DBAL_TYPE_EVENT, 3, 0x55, 0x66, 0x77, 0x88, 3, 4, 5,
            DBAL_TYPE_EVENT, 4, 0x99, 0xAA, 0xBB, 0xCC, 6, 7, 8, 9,
            DBAL_TYPE_EVENT, 5, 0xDD, 0xEE, 0xFF, 0x00, 10, 11, 12, 13, 14};

    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 41));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 41);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 4);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(41, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 41));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 41));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TxRepeat->TxRepeatBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*No Ack*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);

    /*Check notification about undelivered messages.*/
    EXPECT_EQ(DummyLostServiceIds[0], 0x1122U);
    EXPECT_EQ(DummyLostCommandIds[0], 0x3344U);
    EXPECT_EQ(DummyLostDataLen[0], 2);
    EXPECT_EQ(DummyLostServiceIds[1], 0x5566U);
    EXPECT_EQ(DummyLostCommandIds[1], 0x7788U);
    EXPECT_EQ(DummyLostDataLen[1], 3);
    EXPECT_EQ(DummyLostServiceIds[2], 0x99AAU);
    EXPECT_EQ(DummyLostCommandIds[2], 0xBBCCU);
    EXPECT_EQ(DummyLostDataLen[2], 4);
    EXPECT_EQ(DummyLostServiceIds[3], 0xDDEEU);
    EXPECT_EQ(DummyLostCommandIds[3], 0xFF00U);
    EXPECT_EQ(DummyLostDataLen[3], 5);
    EXPECT_EQ(DummyLostIndex, 4);
    /*Reset Values*/
    memset(DummyLostServiceIds, 0, sizeof(DummyLostServiceIds));
    memset(DummyLostCommandIds, 0, sizeof(DummyLostCommandIds));
    memset(DummyLostDataLen, 0, sizeof(DummyLostDataLen));
    DummyLostIndex = 0;
}

TEST_F(DBAL_AppLayerTest, SMM_Send_ErrorDisconnectAfterAllRepetitions)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;

    /*Send command response*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, sizeof(Data)));
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 20, 0x9A, 0xBC, 0xDE, 0xF0,
            1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 29));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 29);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 29));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 29));
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, ExpectedBuffer0, 29));/*Data have not changed!!!*/
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 29));
    /*Try adding some event, no influence on repetitions. Still last repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1234U, 0x5678U, Data, 0));
    /*Error, for the last time*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    /*Check notification about undelivered messages.*/
    EXPECT_EQ(DummyLostServiceIds[0], 0x1234U);
    EXPECT_EQ(DummyLostCommandIds[0], 0x5678U);
    EXPECT_EQ(DummyLostDataLen[0], 0);
    EXPECT_EQ(DummyLostServiceIds[1], 0);
    EXPECT_EQ(DummyLostCommandIds[1], 0);
    EXPECT_EQ(DummyLostDataLen[1], 0);
    /*Reset Values*/
    memset(DummyLostServiceIds, 0, sizeof(DummyLostServiceIds));
    memset(DummyLostCommandIds, 0, sizeof(DummyLostCommandIds));
    memset(DummyLostDataLen, 0, sizeof(DummyLostDataLen));
    DummyLostIndex = 0;
}

TEST_F(DBAL_AppLayerTest, SMM_Send_ErrorDisconnectAckTimeout)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearBothIoTxBuffers(MainInstancePtr);

    /*Send command response*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendCmdResponse(0x9ABCU, 0xDEF0U, Data, sizeof(Data)));
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 20, 0x9A, 0xBC, 0xDE, 0xF0,
            1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 29));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 29);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(29, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 29));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 29));
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
}

TEST_F(DBAL_AppLayerTest, SMM_ReceiveCmd)/*Test all inconsistencies when receiving, including corrupt, unexpected and unknown.*/
{
    uint8_t Cmd2Recv[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD, 2, 0x12, 0x34, 0x01, 0x01, 0x56, 0x78,
                                                            DBAL_TYPE_EVENT_ACK, 0, 0x9A, 0xBC, 0xDE, 0xF0};
    uint8_t Query2Recv[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 2, 0x12, 0x34, 0x01, 0x02, 0xAB, 0xCD};
    uint8_t QueryWrongServiceID[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 2, 0x12, 0x35, 0x01, 0x02, 0xAA, 0xBB};
    uint8_t QueryWrongCommandID[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 2, 0x12, 0x34, 0x01, 0x03, 0xCC, 0xDD};
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(MainInstancePtr);

    /*Error, while reception via response receive handler*/
    /*Wrong Address*/
    Cmd2Recv[DBAL_MSG_SENDER] = 0x98U;
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 17U);
    DbalReceive_appLayerResponse(17U, Cmd2Recv);
    Cmd2Recv[DBAL_MSG_SENDER] = SmmAddr;
    /*Too short*/
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 2U);
    DbalReceive_appLayerResponse(2U, Cmd2Recv);
    /*Wrong protocol type.*/
    Cmd2Recv[DBAL_MSG_PROTOCOL_TYPE] = 1;
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 17U);
    DbalReceive_appLayerResponse(17U, Cmd2Recv);
    Cmd2Recv[DBAL_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Commstate not ready*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    DbalReceive_appLayerRequest(17U, Cmd2Recv);

    /*Error, while reception via request receive handler*/
    /*Wrong Address*/
    Cmd2Recv[DBAL_MSG_SENDER] = 0x98U;
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 11U);
    DbalReceive_appLayerRequest(11U, Cmd2Recv);
    Cmd2Recv[DBAL_MSG_SENDER] = SmmAddr;
    /*Too short*/
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 2U);
    DbalReceive_appLayerRequest(2U, Cmd2Recv);
    /*Wrong protocol type.*/
    Cmd2Recv[DBAL_MSG_PROTOCOL_TYPE] = 1;
    expectCall_CorruptReqRespDbus2Frame(Cmd2Recv, 11U);
    DbalReceive_appLayerRequest(11U, Cmd2Recv);
    Cmd2Recv[DBAL_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Commstate not ready*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    DbalReceive_appLayerRequest(11U, Cmd2Recv);

    /*Receive correct Request as such*/
    DummyCmdSmm_ExpectedDataLen = 2U;
    memcpy((void*)DummyCmdSmm_Bytes, &Cmd2Recv[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(11U, Cmd2Recv);
    EXPECT_NE(0, memcmp((const void*)DummyCmdSmm_Bytes, &Cmd2Recv[9], 2U));
    EXPECT_EQ(UINT8_MAX, DummyCmdSmm_ExpectedDataLen);
    /*Repeat with same seqId->is Ignored*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY, 2U);/*Second call for error debug output.*/
    DbalReceive_appLayerRequest(11U, Cmd2Recv);
    /*Receive unexpected Ack message...*/
    Cmd2Recv[DBAL_MSG_SEQID] = 1U;
    DummyCmdSmm_ExpectedDataLen = 2U;
    memcpy((void*)DummyCmdSmm_Bytes, &Cmd2Recv[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_UnexpectedDBalFrame(SmmAddr, DBAL_TYPE_EVENT_ACK, 0x9ABC, 0xDEF0, &Cmd2Recv[17], 0);
    DbalReceive_appLayerResponse(17U, Cmd2Recv);
    EXPECT_NE(0, memcmp((const void*)DummyCmdSmm_Bytes, &Cmd2Recv[9], 2U));
    EXPECT_EQ(UINT8_MAX, DummyCmdSmm_ExpectedDataLen);

    /*Receive some Query*/
    Query2Recv[DBAL_MSG_SEQID] = 2U;
    DummyQuerySmm_ExpectedDataLen = 2U;
    memcpy((void*)DummyQuerySmm_Bytes, &Query2Recv[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(11U, Query2Recv);
    EXPECT_NE(0, memcmp((const void*)DummyQuerySmm_Bytes, &Query2Recv[9], 2U));
    EXPECT_EQ(UINT8_MAX, DummyQuerySmm_ExpectedDataLen);

    /*Receive non existant query*/
    QueryWrongCommandID[DBAL_MSG_SEQID] = 3U;
    expectCall_UnknownDBalFrame(SmmAddr, DBAL_TYPE_QUERY, 0x1234, 0x0103, &QueryWrongCommandID[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(11U, QueryWrongCommandID);

    /*Receive another non existant query*/
    QueryWrongServiceID[DBAL_MSG_SEQID] = 4U;
    expectCall_UnknownDBalFrame(SmmAddr, DBAL_TYPE_QUERY, 0x1235, 0x0102, &QueryWrongServiceID[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(11U, QueryWrongServiceID);

    /*Repeat correct query with old seqId -> Ignored*/
    memcpy((void*)DummyQuerySmm_Bytes, &Query2Recv[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY, 2U);/*Second call for error debug output.*/
    DbalReceive_appLayerRequest(11U, Query2Recv);

    /*Receive some command with command ID for Query->Unknown*/
    Query2Recv[DBAL_MSG_SEQID] = 5U;
    Query2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = DBAL_TYPE_CMD;
    expectCall_UnknownDBalFrame(SmmAddr, DBAL_TYPE_CMD, 0x1234, 0x0102, &Query2Recv[9], 2U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(11U, Query2Recv);
    Query2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = DBAL_TYPE_QUERY;

    /*Receive some Query with wrong overall length of dbus frame(which is longer)->corrupt, but receive dbal frame*/
    Query2Recv[DBAL_MSG_SEQID] = 6U;
    DummyQuerySmm_ExpectedDataLen = 2U;
    memcpy((void*)DummyQuerySmm_Bytes, &Query2Recv[9], 2U);
    expectCall_CorruptReqRespDbus2Frame(Query2Recv, 12U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(12U, Query2Recv);
    EXPECT_NE(0, memcmp((const void*)DummyQuerySmm_Bytes, &Query2Recv[9], 2U));
    EXPECT_EQ(UINT8_MAX, DummyQuerySmm_ExpectedDataLen);

    /*Receive some Query with wrong overall length of dbus frame(which is shorter)->corrupt, and do not receive dbal frame*/
    Query2Recv[DBAL_MSG_SEQID] = 7U;
    expectCall_CorruptReqRespDbus2Frame(Query2Recv, 10U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(10U, Query2Recv);

    /*Receive some Message with no DBAL Frame inside -> corrupt*/
    Query2Recv[DBAL_MSG_SEQID] = 8U;
    expectCall_CorruptReqRespDbus2Frame(Query2Recv, 3U);
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerRequest(3U, Query2Recv);

    /*Unknown DBal Type (No real command then)*/
    Cmd2Recv[DBAL_MSG_SEQID] = 9U;
    Cmd2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = 6;
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_UnknownDBalFrame(SmmAddr, DBAL_TYPE_UNKNOWN, 0x1234, 0x0101, &Cmd2Recv[9], 2U);
    DbalReceive_appLayerRequest(11U, Cmd2Recv);

    /*Unknown DBal Type (No real command then)*/
    Cmd2Recv[DBAL_MSG_SEQID] = 10U;
    Cmd2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = 7;
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_UnknownDBalFrame(SmmAddr, DBAL_TYPE_UNKNOWN, 0x1234, 0x0101, &Cmd2Recv[9], 2U);
    DbalReceive_appLayerRequest(11U, Cmd2Recv);
    Cmd2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = DBAL_TYPE_CMD;
}

TEST_F(DBAL_AppLayerTest, SMM_FinishSendEventBeforeDisable)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    MainInstancePtr->SeqId2Send = UINT8_MAX;
    MainInstancePtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearBothIoTxBuffers(MainInstancePtr);

    /*Send event*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBAL_sendEvent(0x1122U, 0x3344U, Data, 3U));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 3, 0x11, 0x22, 0x33, 0x44, 1, 2, 3};
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 12));
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 12));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Receive DISABLE REQUEST*/
    uint8_t ConReceiveBuffer[DBAL_CON_MSG_LEN] = {SmmAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ConReceiveBuffer);

    /*Cannot send more messages, after receiving DISABLE REQUEST*/
    EXPECT_EQ(false, DBAL_sendEvent(0x1122U, 0x3344U, Data, 5U));
    EXPECT_EQ(false, DBAL_sendCmdResponse(0x1122U, 0x3344U, Data, 6U));
    EXPECT_EQ(false, DBAL_sendQueryResponse(0x1122U, 0x3344U, Data, 7U));

    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 12));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, Dbus2SendBuffer, 12));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*Confirm event, but with payload, which is wrong*/
    uint8_t EventConfirm0[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 1, 0x11, 0x22, 0x33, 0x44, 0xFF};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_UnexpectedDBalFrame(SmmAddr, DBAL_TYPE_EVENT_ACK, 0x1122, 0x3344, &EventConfirm0[9], 1U);
    DbalReceive_appLayerResponse(sizeof(EventConfirm0), EventConfirm0);

    /*Call timer callback, to trigger repetition.*/
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_RESPONSE);
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TransmitBuffer, ExpectedBuffer0, 12));
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 12);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmit_appLayerResponse(12, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(MainInstancePtr->TxRepeat->TxRepeatBuffer, Dbus2SendBuffer, 12));
    EXPECT_EQ(0, memcmp(Dbus2SendBuffer, ExpectedBuffer0, 12));
    /*Clear buffer by confirmation function.*/
    DbalTransmitPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(MainInstancePtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatBuffer[i], 0U);
        EXPECT_EQ(MainInstancePtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(MainInstancePtr->TxRepeat->TxRepeatLen, 0U);
    EXPECT_EQ(MainInstancePtr->TransmitDataLen, 0);
    EXPECT_EQ(MainInstancePtr->DbalFrames2TransmitCnt, 0);
    (void)memset(Dbus2SendBuffer, 0, sizeof(Dbus2SendBuffer));

    /*DISABLE REQUEST now processed*/
    expectCall_ConnectionSm(MainInstancePtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);

    /*Confirm event*/
    uint8_t EventConfirm[] = {SmmAddr, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_EVENT_ACK, 0, 0x11, 0x22, 0x33, 0x44};
    expectCall_GetCommState(MainInstancePtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(EventConfirm), EventConfirm);
    /*Call timer callback. No mocks are called, so event has not been repeated.*/
    DBAL_msgTimerCallback(MainInstancePtr, 0U, 0);
}

#ifdef DBAL_CROSS_CONNECTION
TEST_F(DBAL_AppLayerTest, CR_CheckFailureBeforeInitClient)
{
    const uint8_t Arr1[] = {1U, 2U, 3U, 4U};
    const uint8_t Arr2[] = {5U, 6U, 7U};
    EXPECT_EQ(DBALCR_sendCmd(0U, 1U, 2U, Arr1, (uint8_t)sizeof(Arr1)), false);
    EXPECT_EQ(DBALCR_sendQuery(0U, 3U, 4U, Arr2, (uint8_t)sizeof(Arr2)), false);
}

TEST_F(DBAL_AppLayerTest, CR_CheckFailureBeforeInitServer)
{
    const uint8_t Arr1[] = {1U, 2U, 3U, 4U};
    const uint8_t Arr2[] = {5U, 6U, 7U};
    const uint8_t Arr3[] = {8U, 9U};
    EXPECT_EQ(DBALCR_sendCmdResponse(1U, 1U, 2U, Arr1, (uint8_t)sizeof(Arr1)), false);
    EXPECT_EQ(DBALCR_sendQueryResponse(1U, 3U, 4U, Arr2, (uint8_t)sizeof(Arr2)), false);
    EXPECT_EQ(DBALCR_sendEvent(1U, 5U, 6U, Arr3, (uint8_t)sizeof(Arr3)), false);
}

TEST_F(DBAL_AppLayerTest, CR_Init)
{
    uint8_t TargetAddresses[] = {CrossAddr, CrossAddr1};
    DBALIN_init(TargetAddresses[0], UserInstancesPtr);
    DBALIN_initCrExtensionByIndex(0U, DBALCR_PART_TYPE_CLIENT);
    DBALIN_init(TargetAddresses[1], (UserInstancesPtr+1));
    DBALIN_initCrExtensionByIndex(1U, DBALCR_PART_TYPE_SERVER);

    expectCall_TimerInitCallback(0, &UserInstancesPtr->ConMsgCbInst, DBAL_conMsgTimerCallback, UserInstancesPtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &UserInstancesPtr->ConMsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false, &UserInstancesPtr->ConMsgCbInst);
    expectCall_TimerInitCallback(0, &UserInstancesPtr->MsgCbInst, DBAL_msgTimerCallback, UserInstancesPtr, STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &UserInstancesPtr->MsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_RESPONSE_TIME_MS, STIM_MODE_SINGLE, false, &UserInstancesPtr->MsgCbInst);

    DBAL_appLayerDBus2Init(UserInstancesPtr);

    UserInstancesPtr->IoMsgLostCbArray[0] = dummyCbLostMsg;
    UserInstancesPtr->IoMsgLostCbCounter++;

    expectCall_TimerInitCallback(0, &(UserInstancesPtr+1)->ConMsgCbInst, DBAL_conMsgTimerCallback, (UserInstancesPtr+1), STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &(UserInstancesPtr+1)->ConMsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_DBUS_RECOVERY_TIME_MS, STIM_MODE_SINGLE, false, &(UserInstancesPtr+1)->ConMsgCbInst);
    expectCall_TimerInitCallback(0, &(UserInstancesPtr+1)->MsgCbInst, DBAL_msgTimerCallback, (UserInstancesPtr+1), STIM_STATUS_TRIGGERED);
    expectCall_TimerInit(0, &(UserInstancesPtr+1)->MsgTimer, STIM_PROCESSING_SCHEDULER, (uint32_t)DBAL_RESPONSE_TIME_MS, STIM_MODE_SINGLE, false, &(UserInstancesPtr+1)->MsgCbInst);

    DBAL_appLayerDBus2Init(UserInstancesPtr+1);

    DBALCQ_init();
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionSend_NoError)
{
    /*At start transmit buffer empty.*/
    for(uint8_t i = 0; i < DBAL_CON_MSG_LEN; i++)
    {
        EXPECT_EQ(UserInstancesPtr->ConnectTransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*ENABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    DBAL_sendCommEnableResponse(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*DISABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer2[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer2, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer2[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer2);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer2, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*DISABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableResponse(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer3[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer3, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer3[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer3);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer3, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*PING REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommPingRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer4[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer4, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer4[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer4);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer4, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*PING RESPONSE (connected)*/
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState((UserInstancesPtr+1), DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommPingResponse((UserInstancesPtr+1));
    EXPECT_EQ(DBALCR_ConInst, (UserInstancesPtr+1));
    const uint8_t ExpectedBuffer5[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, ExpectedBuffer5, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer5[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer5);
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, Dbus2SendBuffer5, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionSend_EnableAfterDisable)
{
    /*DISABLE RESPONSE*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableResponse(UserInstancesPtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));

    /*ENABLE REQUEST before confirmation function*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    DBAL_DBUS_Handler_TxFlags[0] = 8; // set manually
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));

    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();

    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    DBAL_DBUS_Handler_TxFlags[0] = 0; // reset manually
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionSend_DisableOverriddenByEnable)
{
    /*DISABLE RESPONSE*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableResponse(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer0[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer0, DBAL_CON_MSG_LEN));
    /*ENABLE REQUEST before picking up data*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer1[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer1, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer0[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer0);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer0, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionEnableReqSend_ErrorDisable)
{
    /*Covered errors are: DLL_ACK_WRONG and DLL_ACK_BUSY*/
    /*ENABLE REQUEST*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*3. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*4. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*5. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*6. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_REJECT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionEnableReqSend_WakeupSentRequest)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*2. Sending attempt by WakeupSentRequest.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_FALSE(DBAL_isNodeToBeWokenUp(CrossAddr));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer1[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer1);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer1, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    /*Another WakeupSentRequest without effect*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    EXPECT_FALSE(DBAL_isNodeToBeWokenUp(CrossAddr));
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionDisableReqSend_ErrorDisable)
{
    /*No Dbus2 Error, but response timeout...*/
    UserInstancesPtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableRequest(UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*3. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*4. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*5. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*6. Delivery ended*/
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_REJECT);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    UserInstancesPtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionEnableReqSend_ErrorHappyEnd)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE REQUEST*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*2. Sending attempt is triggered from timer.*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_conMsgTimerCallback(UserInstancesPtr, 0U, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalSendWakeup();
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*3. Sending attempt is triggered from timer.*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_conMsgTimerCallback(UserInstancesPtr, 0U, 0);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionDisableReqSend_ErrorHappyEnd)
{
    /*No Dbus2 Error, but response timeout...*/
    UserInstancesPtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
    /*ENABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableRequest(UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery ended*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    DBAL_conMsgTimerCallback((void*)UserInstancesPtr, 0, 0);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery ended*/
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    /*Receive Response*/
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {CrossAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    UserInstancesPtr->ConRepeatCnt = 0; /*Would be set by IoConnectionHandling*/
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionEnableRespSend_ErrorNoAck)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*ENABLE RESPONSE*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommEnableResponse(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_REJECT);
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionPingReqSend_ErrorDisable)
{
    /*Covered errors are: DLL_ACK_WRONG and DLL_ACK_BUSY*/
    /*PING REQUEST*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommPingRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*1. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*2. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*3. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*4. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*5. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_WRONG, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    /*Data are picked up by dbus*/
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    (void)memset(Dbus2SendBuffer, 0U, DBAL_CON_MSG_LEN);
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*6. Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_BUSY, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}


TEST_F(DBAL_AppLayerTest, CR_ConnectionPingReqSend_ErrorNoAck)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*PING REQUEST*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommPingRequest(UserInstancesPtr);
    EXPECT_EQ(DBALCR_ConInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_REQUEST, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionPingRespSend_ErrorNoAck)
{
    /*Covered error is: DLL_ACK_NOT_RECEIVED*/
    /*PING RESPONSE*/
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState((UserInstancesPtr+1), DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommPingResponse((UserInstancesPtr+1));
    EXPECT_EQ(DBALCR_ConInst, (UserInstancesPtr+1));
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*Delivery went wrong.*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_ConnectionSm((UserInstancesPtr+1), DBAL_CON_SM_EVENT_DISABLE_SILENT);
    DBAL_setLastSendingStatus(CrossAddr1, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    EXPECT_EQ(DBALCR_ConInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ConnectionReceive)
{
    uint8_t ReceiveBuffer[DBAL_CON_MSG_LEN] = {CrossAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_ENABLE_REQUEST, DBAL_PROTOCOL_VERSION};

    /*Set stuff, that can be reset by received message.*/
    UserInstancesPtr->DbalFrames2TransmitCnt = 0xED;
    UserInstancesPtr->TransmitDataLen = 0xEDU;
    memset(UserInstancesPtr->TransmitBuffer, 0xEDU, sizeof(UserInstancesPtr->TransmitBuffer));
    /*ComPartner is unknown.*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = 0x57;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = CrossAddr;
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN-1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN-1U, ReceiveBuffer);
    /*Wrong length*/
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN+1U);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN+1U, ReceiveBuffer);
    /*Wrong protocol type*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = 0x02;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Wrong con msg type*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_MSG_TYPE_COUNT;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_REQUEST;
    /*Wrong protocol version*/
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION+1;
    expectCall_CorruptConDbus2Frame(ReceiveBuffer, DBAL_CON_MSG_LEN);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    ReceiveBuffer[DBAL_CON_MSG_PROTOCOL_VERSION] = DBAL_PROTOCOL_VERSION;
    /*Check, that params set at start are unchanged.*/
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0xEDU);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0xEDU);
    for(uint8_t i = 0; i < DBAL_CON_MSG_LEN; i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0xEDU);
    }

    /*Correct Enable Request*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ENABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }

    /*Set stuff, that can be reset by received message.*/
    UserInstancesPtr->DbalFrames2TransmitCnt = 0xED;
    UserInstancesPtr->TransmitDataLen = 0xEDU;
    memset(UserInstancesPtr->TransmitBuffer, 0xEDU, sizeof(UserInstancesPtr->TransmitBuffer));

    /*Correct Enable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_ENABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 0U);
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 0U);
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }

    /*Correct Disable Request*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_REQUEST;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    /*Reset Stuff, which is done by IoConnectionHandling*/
    UserInstancesPtr->DisableReqReceived = false;

    /*Correct Disable Response*/
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_DISABLE_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_ACCEPT);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Ping Request (disconnected)*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = CrossAddr1;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_REQUEST;
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_NOT_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);

    /*Correct Ping Request (connected)*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = CrossAddr1;
    expectCall_DbmDisableInt();
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_GetConnectionState((UserInstancesPtr+1), DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_ConInst, nullptr);
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
    EXPECT_EQ(DBALCR_ConInst, (UserInstancesPtr+1));
    /*Ping Response is sent.*/
    const uint8_t ExpectedBuffer[DBAL_CON_MSG_LEN] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, DBAL_CON_PING_RESPONSE, DBAL_PROTOCOL_VERSION};
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, ExpectedBuffer, DBAL_CON_MSG_LEN));
    /*Data are picked up by dbus*/
    uint8_t Dbus2SendBuffer[DBAL_CON_MSG_LEN] = {0U};
    DbalTransmitCr_appLayerConnection(DBAL_CON_MSG_LEN, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp((UserInstancesPtr+1)->ConnectTransmitBuffer, Dbus2SendBuffer, DBAL_CON_MSG_LEN));
    /*confirmation function*/
    DbalTransmitCrPost_appLayerConnection();
    EXPECT_EQ(DBALCR_ConInst, nullptr);

    /*Correct Ping Response*/
    ReceiveBuffer[DBAL_CON_MSG_SENDER] = CrossAddr;
    ReceiveBuffer[DBAL_CON_MSG_TYPE] = DBAL_CON_PING_RESPONSE;
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ReceiveBuffer);
}

TEST_F(DBAL_AppLayerTest, CR_ClientSendCmd_NoError)/*Include calling with wrong index*/
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Wrong Index*/
    EXPECT_EQ(false, DBALCR_sendCmd(2U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    /*Try to send client messages from server.*/
    EXPECT_EQ(false, DBALCR_sendCmd(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    EXPECT_EQ(false, DBALCR_sendQuery(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Correct Index*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD, 6U, 0x12, 0x34, 0x02, 0x01, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, UserInstancesPtr->TransmitBuffer, 15U));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Confirm Command*/
    uint8_t ReceivedBuffer0[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 2U, 0x12, 0x34, 0x02, 0x01, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DummyCmdRespCr_ExpectedIndex = 0;
    DummyCmdRespCr_ExpectedDataLen = 2U;
    DummyCmdRespCr_Bytes[0] = 1;
    DummyCmdRespCr_Bytes[1] = 2;
    DbalReceive_appLayerResponse(11U, ReceivedBuffer0);
    EXPECT_EQ(DummyCmdRespCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyCmdRespCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((const void*)DummyCmdRespCr_Bytes, &ReceivedBuffer0[9], 2U));

    /*Call timer callback. No mocks are called, so command has not been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);

    /*Unexpected Confirmation of Command, user callback is NOT called.*/
    uint8_t ReceivedBuffer1[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_CMD_ACK, 2U, 0x12, 0x34, 0x02, 0x01, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_UnexpectedDBalFrame(CrossAddr, DBAL_TYPE_CMD_ACK, 0x1234, 0x0201, &ReceivedBuffer1[9], 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(11U, ReceivedBuffer1);

    /*Send some unknown command, to get unknown confirmation*/
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendCmd(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));
    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_CMD, 6U, 0x12, 0x34, 0x02, 0x02, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, UserInstancesPtr->TransmitBuffer, 15U));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);

    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Confirm Command*/
    uint8_t ReceivedBuffer2[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 2, DBAL_TYPE_CMD_ACK, 2U, 0x12, 0x34, 0x02, 0x02, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_CMD_ACK, 0x1234, 0x0202, &ReceivedBuffer2[9], 2);
    DbalReceive_appLayerResponse(11U, ReceivedBuffer2);

    /*Call timer callback. No mocks are called, so command has not been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
}


TEST_F(DBAL_AppLayerTest, CR_ClientSendQuery_Repeat)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Wrong Index*/
    EXPECT_EQ(false, DBALCR_sendQuery(2U, 0x1234U, 0x0202U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    /*Try to send client messages from server.*/
    EXPECT_EQ(false, DBALCR_sendCmd(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    EXPECT_EQ(false, DBALCR_sendQuery(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Correct Index*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 6U, 0x12, 0x34, 0x02, 0x02, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, UserInstancesPtr->TransmitBuffer,  15U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Call timer callback. There was no response, so query is repeated.*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);

    EXPECT_EQ(0, memcmp(ExpectedBuffer0, UserInstancesPtr->TransmitBuffer,  15U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Confirm Query*/
    uint8_t ReceivedBuffer[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY_ACK, 2U, 0x12, 0x34, 0x02, 0x02, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DummyQueryRespCr_ExpectedIndex = 0;
    DummyQueryRespCr_ExpectedDataLen = 2U;
    DummyQueryRespCr_Bytes[0] = 1;
    DummyQueryRespCr_Bytes[1] = 2;
    DbalReceive_appLayerResponse(11U, ReceivedBuffer);
    EXPECT_EQ(DummyQueryRespCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyQueryRespCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((const void*)DummyQueryRespCr_Bytes, &ReceivedBuffer[9], 2U));

    /*Call timer callback. No mocks are called, so command has not been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, CR_ClientSendCmd_ErrorDisconnectAfterAllRepetitions)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Send command response*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    EXPECT_EQ(true, DBALCR_sendCmd(0U, 0x5678U, 0x0709U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD, 6, 0x56, 0x78, 0x07, 0x09, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, ExpectedBuffer0, 15));
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 15);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15));
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(SmmAddr, DLL_ACK_TRANSMISSION_ABORTED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Error*/
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Check notification about undelivered message.*/
    EXPECT_EQ(DummyLostServiceIds[0], 0x5678U);
    EXPECT_EQ(DummyLostCommandIds[0], 0x0709U);
    EXPECT_EQ(DummyLostDataLen[0], 6);
    /*Reset Values*/
    DummyLostServiceIds[0] = 0;
    DummyLostCommandIds[0] = 0;
    DummyLostDataLen[0] = 0;
    DummyLostIndex = 0;
}

TEST_F(DBAL_AppLayerTest, CR_ClientSendCmd_ErrorDisconnectAckTimeout)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Send command response*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    EXPECT_EQ(true, DBALCR_sendQuery(0U, 0x5678U, 0x0709U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 4, 0x56, 0x78, 0x07, 0x09, 1U, 2U, 3U, 4U};
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, ExpectedBuffer0, 13));
    EXPECT_EQ(UserInstancesPtr->TransmitDataLen, 13);
    EXPECT_EQ(UserInstancesPtr->DbalFrames2TransmitCnt, 1);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(13, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 13));
    /*Error*/
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    expectCall_DbmDisableInt();
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_SILENT);
    expectCall_DbmEnableInt();
    DBAL_setLastSendingStatus(CrossAddr, DLL_ACK_NOT_RECEIVED, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_nonDeliverableMessage(DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Check notification about undelivered message.*/
    EXPECT_EQ(DummyLostServiceIds[0], 0x5678U);
    EXPECT_EQ(DummyLostCommandIds[0], 0x0709U);
    EXPECT_EQ(DummyLostDataLen[0], 4);
    /*Reset Values*/
    DummyLostServiceIds[0] = 0;
    DummyLostCommandIds[0] = 0;
    DummyLostDataLen[0] = 0;
    DummyLostIndex = 0;
}

TEST_F(DBAL_AppLayerTest, CR_ClientReceiveEvent)/* Test all inconsistencies when receiving, including corrupt and unknown.*/
{
    uint8_t Event2Recv[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 4, 0x12, 0x34, 0x02, 0x03, 1U, 2U, 3U, 4U};
    uint8_t Event2RecvUnknownServiceId[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 4, 0x12, 0x35, 0x02, 0x03, 1U, 2U, 3U, 4U};
    uint8_t Event2RecvUnknownCommandId[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 4, 0x12, 0x34, 0x02, 0x04, 1U, 2U, 3U, 4U};
    uint8_t EventAck2Send[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x34, 0x02, 0x03};
    uint8_t EventAckUnknownServiceId[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x35, 0x02, 0x03};
    uint8_t EventAckUnknownCommandId[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x34, 0x02, 0x04};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Error, while reception via response receive handler*/
    /*Wrong Address*/
    Event2Recv[DBAL_MSG_SENDER] = 0x98U;
    expectCall_CorruptReqRespDbus2Frame(Event2Recv, 13U);
    DbalReceive_appLayerResponse(13U, Event2Recv);
    Event2Recv[DBAL_MSG_SENDER] = CrossAddr;
    /*Too short*/
    expectCall_CorruptReqRespDbus2Frame(Event2Recv, 2U);
    DbalReceive_appLayerResponse(2U, Event2Recv);
    /*Wrong protocol type.*/
    Event2Recv[DBAL_MSG_PROTOCOL_TYPE] = 1;
    expectCall_CorruptReqRespDbus2Frame(Event2Recv, 13U);
    DbalReceive_appLayerResponse(13U, Event2Recv);
    Event2Recv[DBAL_MSG_PROTOCOL_TYPE] = DBAL_APPLIANCE_LAYER;
    /*Commstate not ready*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    DbalReceive_appLayerResponse(13U, Event2Recv);

    /*Receive correctly*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    DummyEventCr_ExpectedIndex = 0U;
    DummyEventCr_ExpectedDataLen = 4U;
    memcpy((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(13U, Event2Recv);
    EXPECT_EQ(DummyEventCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyEventCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(0, memcmp(EventAck2Send, UserInstancesPtr->TransmitBuffer, 9U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(9, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 9U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Receive Event with unknown Service ID*/
    Event2RecvUnknownServiceId[DBAL_MSG_SEQID] = 1U;
    EventAckUnknownServiceId[DBAL_MSG_SEQID] = 1U;
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_EVENT, 0x1235U, 0x0203U, &Event2RecvUnknownServiceId[9], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(13U, Event2RecvUnknownServiceId);
    EXPECT_EQ(0, memcmp(EventAckUnknownServiceId, UserInstancesPtr->TransmitBuffer, 9U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(9, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 9U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Receive Event with unknown Command ID*/
    Event2RecvUnknownCommandId[DBAL_MSG_SEQID] = 2U;
    EventAckUnknownCommandId[DBAL_MSG_SEQID] = 2U;
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_EVENT, 0x1234U, 0x0204U, &Event2RecvUnknownCommandId[9], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(13U, Event2RecvUnknownCommandId);
    EXPECT_EQ(0, memcmp(EventAckUnknownCommandId, UserInstancesPtr->TransmitBuffer, 9U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(9, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 9U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Receive some frame without dbal frame -->corrupt*/;
    Event2Recv[DBAL_MSG_SEQID] = 3U;
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_CorruptReqRespDbus2Frame(Event2Recv, 3U);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(3U, Event2Recv);

    /*Receive event, where length of DBal frame does not fit length of Dbus2 frame (too long) --> corrupt, but processed*/
    Event2Recv[DBAL_MSG_SEQID] = 4U;
    EventAck2Send[DBAL_MSG_SEQID] = 3U;
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_CorruptReqRespDbus2Frame(Event2Recv, 14U);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    DummyEventCr_ExpectedIndex = 0U;
    DummyEventCr_ExpectedDataLen = 4U;
    memcpy((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(14U, Event2Recv);
    EXPECT_EQ(DummyEventCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyEventCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(0, memcmp(EventAck2Send, UserInstancesPtr->TransmitBuffer, 9U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(9, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 9U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Unknown DBal Type (no real event then)*/
    Event2Recv[DBAL_MSG_SEQID] = 5U;
    Event2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = 7;
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_UNKNOWN, 0x1234, 0x0203, &Event2Recv[9], 4U);
    expectCall_UnexpectedDBalFrame(CrossAddr, DBAL_TYPE_UNKNOWN, 0x1234, 0x0203, &Event2Recv[9], 4U);
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(13U, Event2Recv);
    Event2Recv[DBAL_MSG_FRAME_OFFSET + DBAL_FRAME_DBAL_TYPE] = DBAL_TYPE_EVENT;
}

TEST_F(DBAL_AppLayerTest, CR_ClientReceiveMultipleEvents)
{
    /* Receive multiple events in one message, some of which have same ServiceId and CommandIs, but only one answer.
    * IMPORTANT: Same ServiceId and CommandId are not necessarily next to each other.*/
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearBothIoTxBuffers(UserInstancesPtr);

    uint8_t Event2Recv[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0,
                                       DBAL_TYPE_EVENT, 2, 0x12, 0x35, 0x02, 0x06, 1U, 2U,
                                       DBAL_TYPE_EVENT, 4, 0x12, 0x34, 0x02, 0x03, 1U, 2U, 3U, 4U,
                                       DBAL_TYPE_EVENT, 2, 0x12, 0x35, 0x02, 0x06, 3U, 4U};
    /*Below seqId incremented by two, as Bal Flags are NOT set. Incrementation by one tested in integration test.*/
    uint8_t EventAck2Send[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1,
                                       DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x35, 0x02, 0x06,
                                       DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x34, 0x02, 0x03};
    uint8_t Dbus2SendBuffer[50];

    /*Receive correctly*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 3U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr, 3U);
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_EVENT, 0x1235, 0x0206, &Event2Recv[9], 2U);
    expectCall_UnknownDBalFrame(CrossAddr, DBAL_TYPE_EVENT, 0x1235, 0x0206, &Event2Recv[27], 2U);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP, 2U);
    DummyEventCr_ExpectedIndex = 0U;
    DummyEventCr_ExpectedDataLen = 4U;
    memcpy((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[17], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(29U, Event2Recv);
    EXPECT_EQ(DummyEventCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyEventCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[17], 4));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(0, memcmp(EventAck2Send, UserInstancesPtr->TransmitBuffer, 15U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
}

TEST_F(DBAL_AppLayerTest, CR_ClientReceiveEvent_SendCmd)/*Send command together with ack of event.*/
{
    uint8_t Event2Recv[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 4, 0x12, 0x34, 0x02, 0x03, 1U, 2U, 3U, 4U};
    /*Below seqId incremented by two, as Bal Flags are NOT set. Incrementation by one tested in integration test.*/
    uint8_t EventAck2Send[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x34, 0x02, 0x03,
                                                                               DBAL_TYPE_CMD, 2, 0x12, 0x34, 0x02, 0x01, 3, 4};
    uint8_t CmdAck2Recv[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 1, DBAL_TYPE_CMD_ACK, 2, 0x12, 0x34, 0x02, 0x01, 3, 4};

    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearBothIoTxBuffers(UserInstancesPtr);

    /*Receive correctly*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY, 2U);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    DummyEventCr_ExpectedIndex = 0U;
    DummyEventCr_ExpectedDataLen = 4U;
    memcpy((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4);
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    DbalReceive_appLayerResponse(13U, Event2Recv);
    EXPECT_EQ(DummyEventCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyEventCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((void*)DummyEventCr_Bytes, (const void*)&Event2Recv[9], 4));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    EXPECT_EQ(true, DBALCR_sendCmd(0U, 0x1234, 0x0201, &EventAck2Send[15], 2U));
    EXPECT_EQ(0, memcmp(EventAck2Send, UserInstancesPtr->TransmitBuffer, 17U));
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(17, Dbus2SendBuffer);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    EXPECT_EQ(memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 17U), 0);
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerResponse();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DummyCmdRespCr_ExpectedIndex = 0U;
    DummyCmdRespCr_ExpectedDataLen = 2U;
    memcpy((void*)DummyCmdRespCr_Bytes, (const void*)&CmdAck2Recv[9], 2);
    DbalReceive_appLayerResponse(11U, CmdAck2Recv);
    EXPECT_EQ(DummyCmdRespCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyCmdRespCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((void*)DummyCmdRespCr_Bytes, (const void*)&CmdAck2Recv[9], 2));

    /*Call timer callback. No mocks are called, so event has not been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
}

TEST_F(DBAL_AppLayerTest, CR_ClientFinishSendQueryBeforeDisable)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U};
    uint8_t Dbus2SendBuffer[50];
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);

    /*Wrong Index*/
    EXPECT_EQ(false, DBALCR_sendQuery(2U, 0x1234U, 0x0202U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    /*Try to send client messages from server.*/
    EXPECT_EQ(false, DBALCR_sendCmd(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    EXPECT_EQ(false, DBALCR_sendQuery(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Correct Index*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    const uint8_t ExpectedBuffer[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY, 6U, 0x12, 0x34, 0x02, 0x02, 1U, 2U, 3U, 4U, 5U, 6U};
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Receive DISABLE REQUEST*/
    uint8_t ConReceiveBuffer[DBAL_CON_MSG_LEN] = {CrossAddr, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ConReceiveBuffer);

    /*Call timer callback. There was no response, so query is repeated.*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Data are picked up by dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, Dbus2SendBuffer);
    EXPECT_EQ(0, memcmp(UserInstancesPtr->TransmitBuffer, Dbus2SendBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    for(uint8_t i = 0; i < sizeof(UserInstancesPtr->TransmitBuffer); i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0U);
    }
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);

    /*Repetition not possible, as long as old command not confirmed*/
    EXPECT_EQ(false, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQuery(0U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*DISABLE REQUEST now processed*/
    expectCall_ConnectionSm(UserInstancesPtr, DBAL_CON_SM_EVENT_DISABLE_REQUEST);

    /*Confirm Query*/
    uint8_t ReceivedBuffer[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_QUERY_ACK, 2U, 0x12, 0x34, 0x02, 0x02, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DummyQueryRespCr_ExpectedIndex = 0;
    DummyQueryRespCr_ExpectedDataLen = 2U;
    DummyQueryRespCr_Bytes[0] = 1;
    DummyQueryRespCr_Bytes[1] = 2;
    DbalReceive_appLayerResponse(11U, ReceivedBuffer);
    EXPECT_EQ(DummyQueryRespCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyQueryRespCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((const void*)DummyQueryRespCr_Bytes, &ReceivedBuffer[9], 2U));

    /*Call timer callback. No mocks are called, so command has not been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);

    /*Reset Stuff, which is done by IoConnectionHandling*/
    UserInstancesPtr->DisableReqReceived = false;
}

TEST_F(DBAL_AppLayerTest, CR_ServerSend_RightWrongIndex)
{
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    (UserInstancesPtr+1)->SeqId2Send = 0;
    (UserInstancesPtr+1)->LastSeqIdReceived = UINT8_MAX;
    /*Stop in function DBAL_ioDbusHandler_send assuming commstate is NOT_READY.
     * Do NOT go through all the details. That has been done when testing same stuff with smm connection.*/
    uint8_t Data[] = {1, 2, 3, 4};
    /*Command Response*/
    /*wrong index*/
    EXPECT_EQ(false, DBALCR_sendCmdResponse(2U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*good index*/
    expectCall_DbmDisableInt();
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBALCR_sendCmdResponse(1U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*Query Response*/
    /*wrong index*/
    EXPECT_EQ(false, DBALCR_sendQueryResponse(2U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*good index*/
    expectCall_DbmDisableInt();
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBALCR_sendQueryResponse(1U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*Event*/
    /*wrong index*/
    EXPECT_EQ(false, DBALCR_sendEvent(2U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*good index*/
    expectCall_DbmDisableInt();
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_NOT_READY, 2U);/*Second call for error debug output.*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(false, DBALCR_sendEvent(1U, 0x1234, 0x5678, Data, sizeof(Data)));
    /*Try to send the same server messages from client.
     * -->Rejected at once, without expectCall_GetCommState.*/
    EXPECT_EQ(false, DBALCR_sendCmdResponse(0U, 0x1234, 0x5678, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQueryResponse(0U, 0x1234, 0x5678, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendEvent(0U, 0x1234, 0x5678, Data, sizeof(Data)));
}

TEST_F(DBAL_AppLayerTest, CR_ServerCheckInterfaceBlockedWhenDisablePending)
{
    const uint8_t Data[] = {1U, 2U, 3U, 4U, 5U, 6U};
    /*Reset SeqIds*/
    (UserInstancesPtr+1)->SeqId2Send = UINT8_MAX;
    (UserInstancesPtr+1)->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr+1);

    /*Send Event*/
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendEvent(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, (UserInstancesPtr+1));

    /*Receive DISABLE REQUEST*/
    uint8_t ConReceiveBuffer[DBAL_CON_MSG_LEN] = {CrossAddr1, DBAL_APPLIANCE_LAYER, DBAL_CON_DISABLE_REQUEST, DBAL_PROTOCOL_VERSION};
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerConnection(DBAL_CON_MSG_LEN, ConReceiveBuffer);

    /*Sending not possible, as long as disable is pending.*/
    EXPECT_EQ(false, DBALCR_sendEvent(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendCmdResponse(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQueryResponse(1U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Reset Stuff, which is done by IoConnectionHandling*/
    (UserInstancesPtr+1)->DisableReqReceived = false;
    /*Reset other*/
    DBAL_clearMsgs2Repeat(UserInstancesPtr+1);
    DBAL_clearRetryCounters(UserInstancesPtr+1);
    DBALCR_ReqRespInst=nullptr;
    /**************************/
    /*Start all over again!!!!*/
    /**************************/
    /*Reset SeqIds*/
    (UserInstancesPtr+1)->SeqId2Send = UINT8_MAX;
    (UserInstancesPtr+1)->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr+1);

    /*Send Event*/
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendEvent(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, (UserInstancesPtr+1));

    /*Send DISABLE REQUEST*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_CON);
    DBAL_sendCommDisableRequest(UserInstancesPtr+1);
    (UserInstancesPtr+1)->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTING;

    /*Sending not possible, as long as disable is pending.*/
    EXPECT_EQ(false, DBALCR_sendEvent(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendCmdResponse(1U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(false, DBALCR_sendQueryResponse(1U, 0x1234U, 0x0202U, Data, sizeof(Data)));

    /*Reset Stuff, which is done by IoConnectionHandling*/
    (UserInstancesPtr+1)->DisableReqReceived = false;
    (UserInstancesPtr+1)->IoCurrentConnectionState = DBAL_CONNECTIONSTATE_DISCONNECTED;
    /*Reset other*/
    DBAL_clearMsgs2Repeat(UserInstancesPtr+1);
    DBAL_clearRetryCounters(UserInstancesPtr+1);
    DBALCR_ReqRespInst=nullptr;
}

TEST_F(DBAL_AppLayerTest, CR_ClientAndServerSend)
{
    /*Reset SeqIds*/
    UserInstancesPtr->SeqId2Send = UINT8_MAX;
    UserInstancesPtr->LastSeqIdReceived = UINT8_MAX;
    (UserInstancesPtr+1)->SeqId2Send = UINT8_MAX;
    (UserInstancesPtr+1)->LastSeqIdReceived = UINT8_MAX;
    /*Reset Buffer*/
    DBAL_clearIoTransmitBuffer(UserInstancesPtr);
    DBAL_clearIoTransmitBuffer(UserInstancesPtr+1);
    uint8_t Data[] = {1, 2, 3, 4, 5, 6};
    uint8_t SendBuffer[20] = {0U};

    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    /*Send Command*/
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_REQ);
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendCmd(0U, 0x1234U, 0x0201U, Data, sizeof(Data)));
    EXPECT_EQ(DBALCR_ReqRespInst, UserInstancesPtr);
    /*Send Event*/
    /*Extra check of CommState and ConnectionState: Reading from CrossQueue.*/
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY, 2U);
    expectCall_GetConnectionState((UserInstancesPtr+1), DBAL_CONNECTIONSTATE_CONNECTED);
    expectCall_DbmDisableInt();
    /*No call of BAL_vTransmitMessage*/
    expectCall_DbmEnableInt();
    EXPECT_EQ(true, DBALCR_sendEvent(1U, 0x1234U, 0x0203U, Data, sizeof(Data)));
    const uint8_t ExpectedBuffer0[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD, 6U, 0x12, 0x34, 0x02, 0x01, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(ExpectedBuffer0, UserInstancesPtr->TransmitBuffer, 15U));
    /*Data are picked up by Dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerRequest(15U, SendBuffer);
    EXPECT_EQ(0, memcmp(SendBuffer, UserInstancesPtr->TransmitBuffer, 15U));
    /*Clear buffer by confirmation function - > Send Event now*/
    expectCall_BalTransmitMessage(DBAL_DBUS_HANDLER_SUBSYSTEM, DBAL_MSG_INDEX_APP_LAYER_CROSS_RESP);
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    DbalTransmitCrPost_appLayerRequest();
    EXPECT_EQ(DBALCR_ReqRespInst, (UserInstancesPtr+1));
    const uint8_t ExpectedBuffer1[] = {DBAL_OWN_NODE_ADDRESS, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT, 6U, 0x12, 0x34, 0x02, 0x03, 1U, 2U, 3U, 4U, 5U, 6U};
    EXPECT_EQ(0, memcmp(ExpectedBuffer1, (UserInstancesPtr+1)->TransmitBuffer, 15U));
    for(uint8_t i = 0; i < 15; i++)
    {
        EXPECT_EQ(UserInstancesPtr->TransmitBuffer[i], 0);
    }
    /*Data are picked up by Dbus*/
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalTransmitCr_appLayerResponse(15U, SendBuffer);
    EXPECT_EQ(0, memcmp(SendBuffer, (UserInstancesPtr+1)->TransmitBuffer, 15U));
    /*Clear buffer by confirmation function*/
    expectCall_TriggerPingReqTimer((UserInstancesPtr+1));
    DbalTransmitCrPost_appLayerResponse();
    EXPECT_EQ(DBALCR_ReqRespInst, nullptr);
    for(uint8_t i = 0; i < 15; i++)
    {
        EXPECT_EQ((UserInstancesPtr+1)->TransmitBuffer[i], 0);
    }
    /*Confirm Command*/
    uint8_t ReceivedBuffer0[] = {CrossAddr, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_CMD_ACK, 2U, 0x12, 0x34, 0x02, 0x01, 1U, 2U};
    expectCall_GetCommState(UserInstancesPtr, DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_TriggerPingReqTimer(UserInstancesPtr);
    expectCall_DbmEnableInt();
    DummyCmdRespCr_ExpectedIndex = 0;
    DummyCmdRespCr_ExpectedDataLen = 2U;
    DummyCmdRespCr_Bytes[0] = 1;
    DummyCmdRespCr_Bytes[1] = 2;
    DbalReceive_appLayerResponse(11U, ReceivedBuffer0);
    EXPECT_EQ(DummyCmdRespCr_ExpectedIndex, UINT8_MAX);
    EXPECT_EQ(DummyCmdRespCr_ExpectedDataLen, UINT8_MAX);
    EXPECT_NE(0, memcmp((const void*)DummyCmdRespCr_Bytes, &ReceivedBuffer0[9], 2U));
    /*Confirm Event*/
    uint8_t ReceivedBuffer1[] = {CrossAddr1, DBAL_APPLIANCE_LAYER, 0, DBAL_TYPE_EVENT_ACK, 0, 0x12, 0x34, 0x02, 0x03};
    expectCall_GetCommState((UserInstancesPtr+1), DBAL_COMMSTATE_READY);
    expectCall_DbmDisableInt();
    expectCall_DbmEnableInt();
    DbalReceive_appLayerResponse(sizeof(ReceivedBuffer1), ReceivedBuffer1);
    /*Call timer callback. No mocks are called, so neither command nor event has been repeated.*/
    DBAL_msgTimerCallback(UserInstancesPtr, 0U, 0);
    DBAL_msgTimerCallback((UserInstancesPtr+1), 0U, 0);
}

#endif/*DBAL_CROSS_CONNECTION*/

/******************************************************************************/
/* MOCKS                                                                      */
/******************************************************************************/

std::unique_ptr<BAL_Handler> DBAL_AppLayerTest::BalHandler;
std::unique_ptr<DBPL_Handler> DBAL_AppLayerTest::DbplHandler;
std::unique_ptr<DBM_Handler> DBAL_AppLayerTest::DbmHandler;
std::unique_ptr<STIM_Handler> DBAL_AppLayerTest::StimHandler;
std::unique_ptr<IoConnect_Handler> DBAL_AppLayerTest::IoConnectHandler;
std::unique_ptr<DbalNtf_Handler> DBAL_AppLayerTest::DbalNtfHandler;

void BAL_vTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber)
{
    DBAL_AppLayerTest::BalHandler->BAL_vTransmitMessage(ucSubsystem, ucMessageNumber);
}

void BAL_vCancelTransmitMessage(uint8_t ucSubsystem, uint8_t ucMessageNumber)
{
    DBAL_AppLayerTest::BalHandler->BAL_vCancelTransmitMessage(ucSubsystem, ucMessageNumber);
}

void BAL_vSendWakeupBreak(void)
{
    DBAL_AppLayerTest::BalHandler->BAL_vSendWakeupBreak();
}

void DBPL_vUnNotifyWakeupBreakSent(void)
{
    DBAL_AppLayerTest::DbplHandler->DBPL_vUnNotifyWakeupBreakSent();
}

void DBM_DISABLE_INT(void)
{
    DBAL_AppLayerTest::DbmHandler->DBM_DISABLE_INT();
}

void DBM_ENABLE_INT(void)
{
    DBAL_AppLayerTest::DbmHandler->DBM_ENABLE_INT();
}

int32_t STDCB_InitCallback(struct STDCB_Callback *cbPtr, STDCB_CallbackFunction_t funcPtr, void *objPtr, uint32_t eventMask)
{
    return DBAL_AppLayerTest::StimHandler->STDCB_InitCallback(cbPtr, funcPtr, objPtr, eventMask);
}

int32_t STIM_InitTimer(struct STIM_Timer *timer, enum STIM_Processing processingType,
    STIM_Time_t targetTime_ms, enum STIM_Mode timerMode, bool immediateStart, struct STDCB_Callback *callback)
{
    return DBAL_AppLayerTest::StimHandler->STIM_InitTimer(timer, processingType, targetTime_ms, timerMode, immediateStart, callback);
}

enum DBAL_CommState DBAL_getCommStateByInstance(const struct DBAL_Instance* const Inst)
{
    return DBAL_AppLayerTest::IoConnectHandler->DBAL_getCommStateByInstance(Inst);
}

enum DBAL_ConnectionState DBAL_getConnectionStateByInstance(const struct DBAL_Instance* const Inst)
{
    return DBAL_AppLayerTest::IoConnectHandler->DBAL_getConnectionStateByInstance(Inst);
}

void DBAL_connectionSm(struct DBAL_Instance* const Inst, enum DBAL_ConnectionSmEvent Action)
{
    DBAL_AppLayerTest::IoConnectHandler->DBAL_connectionSm(Inst, Action);
}

void DBALCR_triggerPingReqTimerWithStdTime(const struct DBAL_Instance* const Inst)
{
    DBAL_AppLayerTest::IoConnectHandler->DBALCR_triggerPingReqTimerWithStdTime(Inst);
}

void DBAL_ntfUnknownDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    DBAL_AppLayerTest::DbalNtfHandler->DBAL_ntfUnknownDBalFrameReceived(SenderNodeAddress, DBalType, ServiceId, CommandId, Bytes, DataLen);
}

void DBAL_ntfUnexpectedDBalFrameReceived(uint8_t SenderNodeAddress, enum DBAL_MessageType DBalType, uint16_t ServiceId, uint16_t CommandId, const uint8_t* const Bytes, uint8_t DataLen)
{
    DBAL_AppLayerTest::DbalNtfHandler->DBAL_ntfUnexpectedDBalFrameReceived(SenderNodeAddress, DBalType, ServiceId, CommandId, Bytes, DataLen);
}

void DBAL_ntfCorruptReqRespDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen)
{
    DBAL_AppLayerTest::DbalNtfHandler->DBAL_ntfCorruptReqRespDbus2FrameReceived(Bytes, DataLen);
}

void DBAL_ntfCorruptConDbus2FrameReceived(const uint8_t* const Bytes, uint8_t DataLen)
{
    DBAL_AppLayerTest::DbalNtfHandler->DBAL_ntfCorruptConDbus2FrameReceived(Bytes, DataLen);
}

void DBAL_storeTargetAddress(uint8_t TargetAddress)
{
    DBAL_AppLayerTest::DbalNtfHandler->DBAL_storeTargetAddress(TargetAddress);
}


