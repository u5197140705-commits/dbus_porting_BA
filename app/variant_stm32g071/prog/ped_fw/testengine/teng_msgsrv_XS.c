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

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/

/** \file
 *  \brief Application specific implementation and configuration for test message server.
 *  \remarks It is mandatory to set the compiler switch -DTENG_INCLUDED in your makefile to have
 *           this file compiled and  bind to the executable binary image.
 *
 *  In case you are not actually testing your software, just remove the compiler switch 
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"


#ifdef TENG_INCLUDED

#include "LibTypes.h"
#include "dbus/bal.h"
#include "teng_msgsrv.h"
#include "teng_msgsrv_xs.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

uchar   TEng_ucRxMsg       [RXMSGSIZE];
uchar   TEng_ucTxMsg       [TXMSGSIZE];
/** Bus receive object for Teng subsystem */
const TbusReceiveObject TENG_tBALRx[] =
{
   {{0xFF, 0x00, TENG_MSGRX_ID}, TEng_vBALReadMsg}
};


/** Bus transmit object for Teng subsystem */
const TbusTransmitObject TENG_tBALTx[] =
{
   {
      {(uchar)TENG_TXMSGLEN, (uchar)TENG_DEST_ID, (TbusMessageIdentifier)TENG_MSGTX_ID},
       TXMSGSIZE,
       TEng_vBALWriteMsg,
       NULL
   }
};


/** Number of transmit elements in TENG Bus transmit object */
const uchar TENG_ucBALTxElmtsNum = (uchar)UTI_NELEMENTS(TENG_tBALTx);


/** Teng subsystem transmit flag */
uchar TENG_ucBALTxFlag = 0;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void TEng_vBALReadMsg (uchar ucDataLen, uchar *pucMsgData)
{
   /* Read Bus Data */
   (void)TEng_ucReadMsg (ucDataLen, pucMsgData);

   /* Process Message from Bus */
   TEng_ProcessMsg ();
}

void TEng_vBALWriteMsg (uchar ucDataLen, uchar *pucMsgData)
{
   uchar   i;

   /* Take care of transmit buffer overflow */
   if (ucDataLen < TXMSGSIZE)
   {
      ucDataLen = TXMSGSIZE;
   }
   for (i = 0; i < ucDataLen; i++)
   {
      pucMsgData [i] = TEng_ucTxMsg [i];
   }
}

uchar TEng_ucReadMsg     (uchar ucLen, uchar *pucRxData)
{
   uchar   i;

   /* Avoid receive buffer overflow */
   if (ucLen > RXMSGSIZE)
   {
      ucLen = RXMSGSIZE;
   }
   for (i = 0; i < ucLen; i++)
   {
      TEng_ucRxMsg [i] = pucRxData [i];
   }
   return ucLen;
}//lint !e818   pucRxData cannot be const, interface is fixed for all service functions

void TEng_vWriteMsg    (uchar ucLen, uchar *pucMsgTx)
{
   BAL_vTransmitMessage(TENG_SUBSYSTEM, 0);
   (void)*pucMsgTx;
   (void)ucLen;
}//lint !e818   pucMsgTx cannot be const, interface is fixed for all service functions

#endif
