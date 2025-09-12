#ifndef DBUS2_HSI_ADAPTOR_H__
#define DBUS2_HSI_ADAPTOR_H__

/** \file
 * \brief Connection between Dbuspresentation and HSI layer
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"
#include "LibTypes.h"

#ifdef DOXY_DEVELOPERS_DOC
/**
Compiler switch, which is defined for Update Service Messages with HSI.
*/
#define DBUS2_UPDATE_HSI
#endif

#ifdef DBUS2_UPDATE_HSI


#ifndef __MESSAGE_INPUT_BUFFER_SIZE
#error "Parameter __MESSAGE_INPUT_BUFFER_SIZE must be set to make receive buffer bigger than standard for update functionality."
#endif

/**
Minimum length of an HSI Dbus Message (in Particular HSIProtocolRequest) if the only parameter is "sender address".
*/
#define DBUS2_MIN_LENGTH_OF_MSG_TO_BE_RESPONDED 7U
/**
Receive additional commands, while HSI is in the Dbus buffer and is being processed.
*/
#define DBUS2_SPACE_FOR_OTHER_CMD_THAN_HSI      10U
/**
Max length of HSI frame, for receiving and transmission
 */
#define DBUS2_MAX_HSI_RECEIVE_LEN               (__MESSAGE_INPUT_BUFFER_SIZE - (DBUS2_MIN_LENGTH_OF_MSG_TO_BE_RESPONDED + DBUS2_SPACE_FOR_OTHER_CMD_THAN_HSI))
#define DBUS2_MAX_HSI_TRANSMIT_LEN              DLL_TRANSMIT_BUFFER_DATA_LENGTH
/** \brief          Is sent, after an HsiProtocolRequest has been processed
 *
 * \param vData     Pointer to payload
 * \param ucDataLen Length of payload
 */
void DBPL_vSendHsiProtocolResponse(const void* const vData, uint8_t ucDataLen);

#endif /*DBUS2_UPDATE_HSI*/

#ifdef __cplusplus
}
#endif

#endif/*DBUS2_HSI_ADAPTOR_H__*/
