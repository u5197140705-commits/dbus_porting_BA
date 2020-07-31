/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      TENG
 *  AUTHOR           Smetanka
 ******************************************************************************/

#ifndef TENG_MSGSRV_XS_H
#define TENG_MSGSRV_XS_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file teng_msgsrv_XS.h
 *
 *  \brief Application specific definitions and declarations for test msg server
 *
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "libtypes.h"
#include "dbus/bustypes.h"

/** TEng subsystem */
#if defined (__TESTMSGSRV_SUBNODE_ADDRESS)
#define TENG_SUBSYSTEM        (__TESTMSGSRV_SUBNODE_ADDRESS)
#else
#define TENG_SUBSYSTEM        (10)
#endif


/** Message ID Base to use
\remarks
   - This ID is given by the user as parameter when invoking the test engine
   - It is assumed that TENG_MSGID_BASE + 1 is free as weel, as it will be used
     messages transfers to host
 */
#define TENG_MSGID_BASE       0x9000U


/** TEng Target Address
\remarks
   Host PC address range vary from 0xC to 0xF. Make sure to pick one in this range.
*/
#define TENG_DEST_ADDR        0xCU

/** TEng transmit message length */
#define TENG_TXMSGLEN         (TXMSGSIZE + 2U)
/** TEng Message Rx ID */
#define TENG_MSGRX_ID         TENG_MSGID_BASE


/** TEng Message Tx ID */
#define TENG_MSGTX_ID         (TENG_MSGID_BASE + 1U)


/** TEng Destination ID, depends on Destination Address */
#define TENG_DEST_ID          ((TENG_DEST_ADDR << 4U) & 0xF0U)


/** TEng Entry in BAL Bus Object Table
\remark
   Add this macro at the very end of the bus object 
   table to support test engine
\b Example
\code
const TbusObjectTable BAL_tBusObject[] = 
{
   
#ifdef TENG_INCLUDED
TENG_BAL_BUSOBJ_ENTRY
#endif
}
\endcode
*/

#define TENG_BAL_BUSOBJ_ENTRY ,{TENG_SUBSYSTEM, (TbusReceiveObject *)TENG_tBALRx, \
                                (TbusTransmitObject *)TENG_tBALTx,                \
                                &TENG_ucBALTxElmtsNum, &TENG_ucBALTxFlag}

/* TYPE DEFINITIONS ***********************************************************************************/

/* DECLARATIONS **************************************************************************************/

/** Bus receive object for Teng subsystem */
extern const TbusReceiveObject TENG_tBALRx[];

/** Bus transmit object for Teng subsystem */
extern const TbusTransmitObject TENG_tBALTx[];

/** Number of transmit elements in TENG Bus transmit object */
extern const uchar TENG_ucBALTxElmtsNum;

/** Teng subsystem transmit flag */
extern uchar TENG_ucBALTxFlag;

// ------------------------------------------------------------------
// Message buffers : one per direction
// ------------------------------------------------------------------

extern  uchar   TEng_ucRxMsg[];
extern  uchar   TEng_ucTxMsg[];


/* GLOBAL FUNCTIONS **********************************************************************************/

/**
   TEng callback function to handle Bus Rx Messages.

\param ucDataLen  - Length of Bus Message Payload Data
\param pucMsgData - Pointer to Bus message Payload Data
\return
   none
\remark
   This callback routine is used by the DBUS Bus Receive Object.
*/
extern void TEng_vBALReadMsg (uchar ucDataLen, uchar *pucMsgData);


/**
   TEng callback function to handle Bus Tx Messages.

\param ucDataLen  - Length of Bus Message Payload Data
\param pucMsgData - Pointer to Bus message Payload Data
\return
   none
\remark
   This callback routine is used by the DBUS Bus Transmit Object.
*/
extern void TEng_vBALWriteMsg (uchar ucDataLen, uchar *pucMsgData);
/**
   TEng callback function to handle confirmation of 
   Message transmission.

\param
   none
\return
   none
\remark
   This callback routine is used by the DBUS Bus Transmit Object.
*/
extern void TEng_vBALConfirmMsgTx (void);

extern  uchar TEng_ucReadMsg (uchar ucLen, uchar *pucRxData);


extern void TEng_vWriteMsg (uchar ucLen, uchar *pucMsgTx);


#ifdef __cplusplus
}
#endif

#endif
