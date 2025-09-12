/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          DBus Application Layer
 *  COMP_ABBREV      DBAL
 ******************************************************************************/

#ifndef DBALCFG_H__
#define DBALCFG_H__

// SUB-SYSTEM address for DBal Layer for this MCU
#define DBAL_DBUS_HANDLER_SUBSYSTEM         0x3

// number of retries when message was not delivered/responded (multiplied also by BAL_MAX_TRANSMISSION_RETRIES_AFTER_MISSING_ACKNOWLEDGE from balXs.c)
#define DBAL_DBUS_RETRY_MAX                 5

// delay between repetition of transmission in case the connection message was not acknowledged
#define DBAL_DBUS_RECOVERY_TIME_MS          500

// delay after which command/query/event is repeated, if no response
#define DBAL_RESPONSE_TIME_MS               200

// number of commands, queries and events, that can be queued at a time to be resent to a communication partner
// if that partner has not responded to the message in a correct way
#define DBAL_MAX_MSGS2REPEAT                4

// maximum size of payload to be sent in commands, queries and events which are to repeated, if the communication partner has not responded
// each DBal frame contains the standard parameters (sender, msgType; dbalType, seqId, serviceId, commandId) and the payload defined by users
#define DBAL_MAX_DATALEN_2REPEAT            10


#endif /* DBALXS_H */
