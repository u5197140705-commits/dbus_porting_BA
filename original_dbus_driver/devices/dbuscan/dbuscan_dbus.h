/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCDBUS
 ******************************************************************************/


#ifndef DBUSCAN_DBUS_H
#define DBUSCAN_DBUS_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public definitions and declarations related to DBus part of the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_types.h"
#include "bustypes.h"
#include "mcal/mcal_types.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
PACKED struct DBCDBUS_Address
{
    uint8_t  nodeAddress;
    uint16_t subsystMask;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern const struct DBCDBUS_Address DBCDBUS_addressList[DBC_DBUS_NF_COUNT];
extern DBC_WriteBuf_t DBCDBUS_txBuf;                  ///< Buffer for writing data to the DBusCAN chip over SPI
#ifdef DBUSCAN_WITH_BBL_SPI
extern MCAL_CallbackFunction_t DBCDBUS_cbFunctionPtr; ///< Pointer to callback function handling DBus traffic or interrupts from the DbusCAN chip
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef DBUSCAN_SPI_CRC_USED
/** \brief   Notifies user that DBus message received by the DBusCAN chip has been lost.
 *
 *  \details This function informs user about situation when a DBus message is successfully received
 *           by the DBusCAN chip but due to communication error on SPI bus the message is
 *           not transferred to the MCU and thus not handled. As a result, the message is considered
 *           as lost.
*/
extern void DBCDBUS_notifyRxMsgLost(void);
#endif // DBUSCAN_SPI_CRC_USED


/** \brief   Handles interrupts from the DBusCAN chip.
 *
 *  \details All interrupt flags from the DBusCAN chip
 *           are read, cleared and handled if needed.
 *
 *  \note    This function is used internally by DBus layers.
 *
 *  \return  true if handling of interrupt flags is complete, otherwise false
 */
extern bool DBCDBUS_handleIrq(void);


/** \brief   Sends DBus frame via the DBusCAN chip (over DBus line).
 *
 *  \note    This function is used internally by DBus layers.
 *
 *  \return  false if the request to send the DBus message is being processed, true if it is complete
 */
extern bool DBCDBUS_sendFrame(void);


/** \brief   Reads received DBus frame from the DBusCAN chip.
 *
 *  \note    This function is used internally by DBus layers.
 *
 *  \return  pointer to received DBus frame
 */
extern TbusMessage* DBCDBUS_readFrame(void);


/** \brief   Returns information whether wake-up pulse transmission (over DBus line) is completed.
 *
 *  \return  true - wake-up pulse transmitted, false - wake-up pulse transmit ongoing
 */
extern bool DBCDBUS_isWakeUpPulseSent(void);


/** \brief   Sends wake-up pulse via the DBusCAN chip (over DBus line).
 *
 *  \note    This function is used internally by BAL_vSendWakeupBreak function.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_sendWakeUpPulse(void);


/** \brief   Unlocks DBus pin of the DBusCAN chip for direct access.
 *
 *  \details Also sets Dbus pin level according to @pinLevel parameter.
 *
 *  \note    This function is used internally by BAL_vSendResetBreak function.
 *
 *  \param   pinLevel: logical level to be set for the DBus pin
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_unlockDbusPin(bool pinLevel);


/** \brief   Locks DBus pin of the DBusCAN chip for direct access.
 *
 *  \note    This function is used internally by BAL_vSendResetBreak function.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_lockDbusPin(void);


/** \brief   Sets logical level of the DBus pin.
 *
 *  \note    Works only if DBus pin is unlocked for access via @DBCDBUS_unlockDbusPin.
             This function is used internally by BAL_vSendResetBreak function.
 *
 *  \param   pinLevel: logical level to be set
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_setDbusPin(bool pinLevel);


/** \brief   Sets multiple DBus addresses in a chip (using node filters).
 *
 *  \note    This function is used internally by DBCDRV_init function.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_setMultipleAddresses(void);


#ifndef RTOS_DBUS_EVENTDRIVEN
/** \brief   Informs whether transmission of DBus message from the DBusCAN chip is completed.
 *
 *  \note    This function is used internally, not intended to be used by application.
 *
 *  \return  true or false
 */
extern bool DBCDBUS_isMsgTransmitted(void);


#else //!RTOS_DBUS_EVENTDRIVEN
/** \brief   Returns number of reveived messages.
 *
 *  \return  Number of received messages, saved in DBCDBUS_newMsgCount
 */
extern uint8_t DBCDBUS_getNewMsgCount(void);


/** \brief   Returns pointer DBCDBUS_rxFramePtr.
 *
 *  \return  Pointer DBCDBUS_rxFramePtr.
 */
extern const TbusMessage* DBCDBUS_getRxFramePtr(void);
#endif //!RTOS_DBUS_EVENTDRIVEN


/** \brief   Returns acknowledge value received by the DBusCAN chip.
 *
 *  \return  received acknowledge value
 */
extern uint8_t DBCDBUS_getReceivedAck(void);


/** \brief   Enables acknowledge for received message on DBus node (and any subnode) address
 *
 *  \note    This function is used internally by DLL_vSetSilentMode function.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_enableAck(void);


/** \brief   Disables acknowledge for received message on DBus node (and any subnode) address
 *
 *  \note    This function is used internally by DLL_vUnsetSilentMode function.
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDBUS_disableAck(void);


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif

#endif // DBUSCAN_DBUS_H
