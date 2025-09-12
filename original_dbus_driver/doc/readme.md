# Guide to add project specific DBUS messages into MCU Framework

To add project specific DBUS messages, users have to create their own DBus transmit and receive tables. Those tables must be registered into BAL_tBusObject[] in balXs.c config file.</br>
For inspiration and easier understanding, please see implementation examples in the [links below.](#2-implementation-of-project-specific-messages)

## 1. Prototype data structures

The file [prog\dbus\bustypes.h](../bustypes.h) contains `TbusTransmitObject` and `TbusReceiveObject` data structures.
This header file defines the general variable types used by the DBus.

### 1.1 Data structure `TbusReceiveObject`
```c
typedef PACKED struct POSTPACKED _TBusReceiveObject
{
       TbusIdentifier  tBusIdentifier;
       TbusService     tServiceFunction;
} TbusReceiveObject;
```
The data structure `TbusReceiveObject` consists of the elements (described below) that are needed to recognize the received DBus message.

### 1.1.1 Elements of the structure `TbusReceiveObject`

- TbusIdentifier tBusIdentifier described [here.](#13-data-structure-tbusidentifier) </br>

- TbusService tServiceFunction structure element:
    - it's a pointer to the function with arguments (uint8_t ucDataLen, uint8_t *pucData)</br>
    - it is called when an application specific DBus message has been received successfully</br>
    - it provides a **pointer to the data payload** of the DBus message, as well as a parameter stating its length, so that the application can check, whether the received data has the expected length


### 1.2 Data structure `TbusTransmitObject`
```c
typedef PACKED struct POSTPACKED _TBusTransmitObject
{
       TbusIdentifier          tBusIdentifier;
       uint8_t                 ucDataLen;
       TbusService             tServiceFunction;
       TbusConfirmationService tConfirmationFunction;
}TbusTransmitObject;
```
The data structure `TbusTransmitObject` consists of the elements (described below) that are needed to transmit user-specific DBus messages.

### 1.2.1 Elements of the structure `TbusTransmitObject`

- TbusIdentifier tBusIdentifier described [here.](#13-data-structure-tbusidentifier) </br>
- uint8_t ucDataLen: length of the data payload to be sent in bytes</br>
- TbusService tServiceFunction: the reference to ServiceFunction, where the data is being transferred from the application to the bus.
    - it's a pointer to the function with arguments (uint8_t ucDataLen, uint8_t *pucData)</br>
    - it is called when an application specific DBus message is triggered to be transmitted over DBus</br>
    - it provides a **pointer to the DBus transmit buffer** where the user payload data should be stored, as well as the predefined payload length
- TbusConfirmationService tConfirmationFunction: the (optional) pointer to ConfirmationFunction, where the application can be notified about a successful DBus message transmission</br>

### 1.3 Data structure `tBusIdentifier`
```c
typedef PACKED struct POSTPACKED _TBusIdentifier
{
       uint8_t                  ucMessageLength;
       uint8_t                  ucTargetAddress;
       TbusMessageIdentifier  tMessageIdentifier;
}TbusIdentifier;
```
This structure contains general information about the DBus frame; including the length of the message, the target address, and the message identifier.

### 1.3.1 Elements of the structure `tBusIdentifier`

- **uint8_t ucMessageLength:** expected message length

- **uint8_t ucTargetAddress**: address of corresponding communication partner (to which the message will be sent). The main node (address) is placed in the upper nibble, and the addressed subsystem (subnode) is in the lower nibble.
- **TbusMessageIdentifier tMessageIdentifier**: 16 bit message identifier. Identifiers between 0 and 0x7FFF are intended to be project specific and can be used by anyone for any project-specific message. 

Identifiers with values between 0x8000 and 0xFFFF are reserved for generic purposes. 
To reserve a message identifier range for generic use, please contact the current leading developer of the component DBus, providing according written documentation of the purpose. 
To see all service and system messages, look at the documentation [BSH General Bus Specification D-Bus-2.2](D-Bus-2_2-PresentVersion.pdf) in chapter **5.2 Messages**, the table 14: "Message Distribution". 

## 2. Implementation of project-specific messages

The two files below contain an implementation example of project-specific receive/transmit tables, service and confirmation functions, and corresponding definitions.

Link [DBus_user_interface.c](../example/DBus_user_interface/DBus_user_interface.c)

Link [DBus_user_interface.h](../example/DBus_user_interface/DBus_user_interface.h)

It is recommended to **create custom files** and fill tables with lists of messages as follows:

Create [`TbusTransmitObject[]`](#12-data-structure-tbustransmitobject) variable that contains a list of messages which can be transmitted over the DBus.

Create [`TbusReceiveObject[]`](#11-data-structure-tbusreceiveobject) variable that contains a list of messages that can be received from the DBus.

Create other necessary elements described in [TbusObjectTable](#21-data-structure-tbusobjecttable)

- **It is important to indicate the end of TbusReceiveObject. For this purpose, the last entry's message length must be set to MSB_BYTE value (see example file [DBus_user_interface.c](../example/DBus_user_interface/DBus_user_interface.c)).**

Add the receive/transmit tables from your project specific file, such as e.g. DBus_user_interface.c to `TbusObjectTable` in balXs.c. Link to example [prog\dbus\balxs.c](../example/dbus/balXs.c)

### 2.1 Data structure `TbusObjectTable`

The variable `TbusObjectTable BAL_tBusObject[]` contains lists of messages which can be received and sent. Each subsystem contains exactly one entry, pointing to one list of messages to be received and one list of messages to be sent by the given subsystem.
After successful reception, the received message ID is searched only in the subsystem defined in the received message header, with two exceptions:
- Service messages are searched in subsystem zero as well.
- All implemented subsystems are searched for proper broadcast messages (sent to address 0x00).

#### Elements of structure `TbusObjectTable`

- **uint8_t ucSubsystem:** a subsystem ID is a 4-bit subsystem identifier. Together with the main node, which is in the upper nibble, this creates the unique 8-bit target address.

- **const [`TbusReceiveObject[]`](#11-data-structure-tbusreceiveobject) *ptReceiveTable:** a list of messages that can be received

- **const [`TbusTransmitObject[]`](#12-data-structure-tbustransmitobject) *ptBusTransmitTable:** a list of messages that can be transmitted

- **const uint8_t *pucNumberOfElementsInTransmitTable:** number of messages defined in the corresponding transmit table

- **uint8_t *pucMessageToTransmitBits:** the transmit flags consist of one bit for each message, keeping information about which messages will be transmitted. A subsystem with up to 8 messages needs one byte for the transmit flags.
Subsystems with up to 16 messages needs two bytes, etc. Least significant bit represents the first message in a subsystem (index 0 in the transmission table).


## 3. Transmission of DBus messages

To transmit a DBus message user has to use the function BAL_vTransmitMessage(), which is defined in the file bal.c.

### 3.1 Arguments of function BAL_vTransmitMessage:

- **uint8_t ucSubsystem:** Subsystem ID - presented in the table BAL_tBusObject[] within in the file balXs.c. It defines which transmit table from BAL_tBusObject should be used.

- **uint8_t ucMessageNumber:** Index of a message in the transmit table of the given subsystem.
