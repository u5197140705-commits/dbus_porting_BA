# DBal - DBus2 Application Layer

## Intro
As part of the new system architecture (SyMaNa, which will not be explained here), a general protocol for communication between the electronics has been defined. All communication is based on the request-response pattern. There are basically three types of communication available, command, query and event. Furthermore, it has been decided that only a "Client" <-> "Server" communication is possible. In a connection between an ecu and the Systemmaster, the ecu is always the server and the Systemmaster is the client.

#### DBal Is Responsible For:
Communication between different electronics. That means there is some communication handling and functions are provided for easily accessing the Bsh-Dbus2. 

#### ... DBal Is NOT:
We do not know anything of powerstates. We only have the condition that each electronic must be awakenable via break and always be able to receive messages, independently of the current powerstate.

#### What Is This Document About ?
It attempts to explain, how to use the DBal implementation for MCU Framework.<br>
You can acquire more details by: <br>

 - reading our [example application](https://scr.bsh-sdd.com/projects/SYMANA/repos/mcu_samples/).
 - reading the doxygen comments from the [folder](..) where Dbal is implemented. 
   (Use target "make doxy" to produce html pages, which are easier to read than the comments in the code.)

#### What This Document Is NOT
It DOES NOT contain a complete description of the concept behind DBal or the specification.<br>
Please read the [specification](https://production.polarion.bshg.com/polarion/#/project/General_eSystem_Requirements_and_Architecture/wiki/40%20-%20Architecture%20_%20Design/SDS_DBus2_Communication_Specification) BEFORE YOU CONTINUE. Everybody at BSH should have read access to this document.
If not, ask one of the [key users](https://production.polarion.bshg.com/polarion/#/project/SupportPortal/home) of the document for the read access.

## What Are The Benefits Of Using This Layer?

- Clear communication pattern: User only has to focus on api, rather then on technology stack.
- Robustness: Automatically establishing handling of connection e.g. repetitions, timeouts, ... 
- Easy to test.
- Fully integrated into symana - design your software with io functions.
- Simple API for communication between Systemmaster and ecu's.

## Terms And Abbreviations
*SYMANA:* Systemmaster new architecture; It is a complete concept, how to organize/develop a BSH device consisting of a Systemmaster and microcontrollers. The DBal layer is responsible for the communication in devices, which are to be assembled according to the SYMANA concept. <br>
*SYMANA PLATFORM:* Symana [platform](https://scr.bsh-sdd.com/projects/SYMANA/repos/platform/browse) repository provides to you all helpful libraries and components for developing your domain application on the Systemmaster. The most interesting parts (unless you want develop some homeconnect ui) are the directories generic (logging, timers,...) appliance_domain and ecu_io. The last two are the frameworks for developing your application.<br>
*MCU Framework:* The [MCU Framework](https://github-bshg.boschdevcloud.com/Generic/Framework) is the counter part for the ECU development. <br>
*Systemmaster:* The *master* in our system, both in communication and logically. Typically, the Systemmaster is the center of communication.<br>
*ECU:* Electronic control unit. We use this name for nearly all electronics except for the Systemmaster. SYMANA expects the software which is written for the ECU to be based on the MCU Framework (see above).<br>
*Cross Communication:* Direct communication between ECUs, without Systemmaster in between, also called "ECU to ECU communication". Must remain a complete exception in SyMaNa systems.<br>

## Interface
You can use the generated doxy documentation or the direct link below to get more information about the functionality which is provided: <br>
<br>
[Interface](../BshDBus2AppLayer.h) <br>
[Interface For Ecu To EcuCommunication](../BshDBus2AppLayerCross.h) <br>
<br>
Basically, there are three types of DBal frames for communication: Commands, queries and events. All three are automatically repeated, 
if not responded by the other side (command ack, query ack, event ack).<br>
The client can send commands and queries to steer and supervise the server, while the server can use events to notify the client of occured changes. <br> 
<br>
Furthermore, there are functions provided in order to enable or disable the communication and to query the current communication status. <br>
Users may register methods to be notified about any communication status change as well. 

## How To Use
Start with the software architecture and design. After shaping your software into io functionalities, define services and command Ids. Each functionality must be defined as a command, query or event. <br>
Depending on the purpose, you must use the correct type of message: <br>

- Use commands, if the client wants something to be executed on the server.
- A query shall be used to get some information from the server.
- Events are intended to inform the client about logical changes, which have occured on the server.

Once you have a complete concept, you can start with the implementation. <br> 
Hints and basic explanations for this purpose are given in the following part of this chapter. <br> 
<br>
The communication with the Systemmaster (which is always the client in relation to ecu), as well as general aspects, will be discussed first.<br>
In order to provide additional information on cross communication (ecu to ecu), there is a separate [subchapter](#ecu-to-ecu-communication) below.<br>
For good and bad practices, when using DBal, you can find a separate [subchapter](#short-guideline-for-implementation-good-and-bad-practices) below as well.<br>
<br>
At the beginning of implementation, activate the DBal driver in your project the following way: <br>
Using the setup gui from MCU Framework, select DBal support. A define for the activation of the DBal usage and a folder with configuration data for DBal will be created. <br>
<br>
At the start of your program, you have to initialize the DBAL layer (function "DBAL_init") and enable communication (function "DBAL_enable"). <br>
Optionally, you can register one or more callbacks, to be notified about any change in the communication status (function "DBAL_registerCommState")
and about which messages could not be delivered before the communication status changed to "NOT READY" (function "DBAL_registerLostMsg"). 
Alternatively, you may regularly query the communication status (function "DBAL_getCommState"). <br>
If you want to register more than one callback for the communication status and/or undelivered messages, 
adjust the macro DBAL_IO_MAX_CALLBACK_BUFFER_SIZE (which is 1 by default) to a higher value in your build process. <br>
<br>
Furthermore, see to it, that the parameters in the files "DBal_cfg.c/.h" are correctly configured. <br>
<br>
As soon, as the client has booted successfully, the client will automatically seek to build up a stable connection with the server. <br>
(This is the case, if the client application has called the "DBAL_enable" function, or on SMM, an analogic function.) <br>
As soon, as the server has booted successfully, the server will automatically seek to build up a stable connection with the client. <br>
(This is the case, if the server application has called the "DBAL_enable" function.) <br>
<br>
The moment the connection is established (communication status "READY"), you will be able to respond commands and queries and to send events. <br>
In order to be able to process commands and queries, you have to implement "DBAL_ObjectTable": <br>

- It must contain one entry for each ServiceId.
- For the given ServiceId, there is an array of callback functions to receive messages with different CommandIds.
- For each receive callback entry you also define the type of the DBal frame to receive: command, query.
- For DBal frames of the type "event ack", the user does not have to implement a receive callback. Furthermore "event ack" frames are NOT redirected to receive callbacks AT ALL. The DBal driver just notes internally, that the event has been acknowledged correctly and does not have to be repeated.
- Only, if ServiceId, CommandId and the type of the DBal frame are ALL correct, will the receive callback be called.
- If the callback for a message is not implemented or connected incorrectly, you will be notified by a call of "DBAL_ntfUnknownDBalFrameReceived".
<br>
Be aware, that you must respond any command or query immediately. Further requests on the client's side are queued until the previous request has been answered. <br>
<br>
DBal handles communication errors, retries of unanswered and undelivered messages and message counting in order to help to avoid duplicated messages. <br>
Messages, that must be answered are commands, queries and events. <br>
If the communication is down, this will be signaled by an according communication status event. 
(You can trigger the attempt of a recovery of the connection by calling "DBAL_enable".) <br><br>

[A more detailed description of the entire error handling can be found here.](#error-processing)

[A description of a debug interface for DBal can be found here.](#debugging-interface)

Moreover, intermediate data buffers are used inside DBal, so that you don't have to worry about overwriting DBus2 buffers.

### Ecu To Ecu Communication
Ecu to ecu communication must be the absolute exception in SYMANA systems and can be used, if the Systemmaster is sometimes/often hibernating during operation to save energy.<br>
<br>
Define the macro DBAL_CROSS_CONNECTION in your build process. <br>
Define the macro DBAL_CROSS_CONNECT_COUNT in your build process and set its' value to the number of ecus, with which your ecu is to communicate. <br>
**Especially, if DBAL_CROSS_CONNECT_COUNT is larger than 1, you might need to set the parameter DBAL_RESPONSE_TIME_MS in DBal_cfg.h to a much bigger value than the one set by default. (The default value is ideal for the typical case without cross communication.) Depending on the exact value of DBAL_CROSS_CONNECT_COUNT, your baud rate, your bus load, etc., DBAL_RESPONSE_TIME_MS should realistically be somewhere between 300 and 1000.** <br>
**Furthermore, remember to define similar values for DBAL_RESPONSE_TIME_MS timeout on both sides of a given cross connection!**<br>
<br>
Initialize DBal at the start of your program (both "DBAL_init" and "DBALCR_init" have to be called to make the code work properly). <br>
You can activate (function "DBAL_enable") or deativate (function "DBAL_disable") the communication with the Systemmaster at any time, when this is neccessary.<br>
(However, typically the client, which is the Systemmaster, must decide, when to enable or disable the connection with it.)<br>
(Disconnecting might be needed, for example, once you know, that the Systemmaster is going to hibernate.) <br>
In the same way, you can activate (function "DBALCR_enable") or deactivate (function "DBALCR_disable") the communication with other ecus at any time.
(If the Systemmaster is not hibernating and can take charge, most ecu to ecu connections are not needed.) <br>
<br>
For handling of the communication status, you can use similar functions, as in the communication with the Systemmaster (functions "DBALCR_registerCommState",
"DBALCR_registerLostMsg" and "DBALCR_getCommState").<br>
<br>
The connection and error handling in ecu to ecu communication is exactly the same as on the connection to the Systemmmaster.<br>
The relevant parameters to configure can be found in "DBal_cfg.c" under the compiler switch DBAL_CROSS_CONNECTION, in particular:<br>

- Fill the array "DBALCR_ComPartners" with correct (between 0x10 and 0xFF) Dbus2 addresses of the cross communication partners.
  (The array is not constant, so if you do not know the values at compile time, fill it at runtime, before calling "DBALCR_init".)
- Fill the array "DBALCR_RolesTowardPartners" with the information, whether your application will be client or server toward the partners defined in
  "DBALCR_ComPartners". The value "DBALCR_PART_TYPE_NONE" is not valid. (The array is not constant, so if you do not know the values at compile time, fill it at runtime, before calling "DBALCR_init".)
- **The entries in "DBALCR_ComPartners" and "DBALCR_RolesTowardPartners" MUST be in the same order!!!**
- **If you initialize the arrays "DBALCR_ComPartners" and "DBALCR_RolesTowardPartners" at runtime and find that you wish to have less than DBAL_CROSS_CONNECT_COUNT connections, simply fill the remaining values at the END of both ARRAYS with INVALID values.**
- If your ecu is to be a client toward (some of the) other ecus and you want to use the Ping mechanism, set DBALCR_PingTimeMs to an according value. (Look at the doxygen documentation of DBALCR_init for details. The parameter is not constant, so if you do not know the value at compile time, fill it at runtime, before calling "DBALCR_init".)
- Implement "DBALCR_ObjectTable" in order to receive messages in cross communication following the rules below:

    - It must contain one entry for each ServiceId.
    - For the given ServiceId, there is an array of callback functions to receive messages with different CommandIds.
    - For each receive callback entry you also define the type of the DBal frame to receive: command, command ack, query, query ack, event.
    - For DBal frames of the type "event ack", the user does not have to implement a receive callback. Furthermore "event ack" frames are NOT redirected to receive callbacks AT ALL. The DBal driver just notes internally, that the event has been acknowledged correctly and does not have to be repeated.
    - Only, if ServiceId, CommandId and the type of the DBal frame are ALL correct, will the receive callback be called.
    - If the callback for a message is not implemented or connected incorrectly, you will be notified by a call of   "DBAL_ntfUnknownDBalFrameReceived".
    - The parameter "Index" of the receive callback marks the position, in which the sender has been entered in "DBALCR_ComPartners", before calling "DBALCR_init". This way you know, which participant of the cross communication has sent the message.

Furthermore, when wanting to send messages on a DBal cross connection, one must remember the following points: <br>

- Sending of the different types of DBal frames is restricted depending on your role on the given cross connection:
    
    - As a client, you can send commands, queries and acknowledges to events. A client can also ping the server.
    - Acknowledges to events are generated automatically by the DBal driver. The application does NOT need to do that.
    - You CANNOT queue another DBal frame of type "command" or "query", as long as the current one has not been acknowledged by an according response. If you try it nevertheless, you will receive a negative return value. 
    - As a server, you can send events and responses to commands or queries.
    - You define it by the values in the array "DBALCR_RolesTowardPartners", whether you are a server or client.
    - If your application tries to send a kind of DBal frame, which it is not allowed to send, you will receive a negative return value.
- The parameter "Index" in (almost) all functions of the [cross communication interface](../BshDBus2AppLayerCross.h) marks the position, in which the Dbus2 address of the communication partner has been entered in "DBALCR_ComPartners", before calling "DBALCR_init". This way you can be sure, which participant you are addressing. <br>

### Handling Of Detected Break Signals
Because there is currently no generic way to detect Dbus2 wakeup break signals on the Dline, each project has to do that by themselves. <br>
<br>
A detected break signal is to be reported to DBal by calling DBAL_breakSignalDetected. <br>
As a consequence, DBal will arrage the transmission of the "WakeupSentRequest" message and will monitor, whether an "Enable Request" is received
from any of the communication partners, with which no DBal connection is currently active (communication state "READY"). If no "Enable Request" is reveived
within DBAL_DBUS_RECOVERY_TIME_MS, the user is notified about communication state still being "NOT READY" for all these communication partners. <br>

### Short Guideline For Implementation: Good And Bad Practices
When configuring DBal, it is not advisable to set the macros<br>

- DBAL_DBUS_RETRY_MAX
- DBAL_DBUS_RECOVERY_TIME_MS
- DBAL_RESPONSE_TIME_MS
- DBAL_MAX_MSGS2REPEAT
- DBAL_MAX_DATALEN_2REPEAT

in DBal_cfg.h to smaller values than are proposed in the template of this file. <br>
In some projects, values reduced by up to 50% might work fine, but you have to try that out by yourself. <br>
Setting any of the macros above to zero WILL LEAD TO UNDEFINED BEHAVIOR!!! <br>
Please remember to be reasonable as well, when increasing above values. For example, it makes no sense to repeat messages 100 times. <br>
Moreover, **for ecu to ecu communication, DBAL_RESPONSE_TIME_MS might need to be significantly increased. [(see)](#ecu-to-ecu-communication)** <br>
<br>
Also see to it, to fill in "DBAL_ObjectTable" (and  if needed "DBALCR_ObjectTable", "DBALCR_ComPartners", "DBALCR_RolesTowardPartners") correctly.
<br>
<br>
Once your configuration is correct, double check, whether the DBal frames, that have been defined in your project, are unambiguous. <br>
For example, if you defined a command such as<br>

- ToggleStateOfDeviceXY

this is not good. <br>
Due to the properties of Dbus2 and its' over 20 years old interface, one DBal frame might under some circumstances be delivered twice, 
AND with different sequenceId values. Thus, the command will be executed twice. <br>
In such a case, your device will stay in the current state. <br>
It will be much better, if you define two clear commands: <br>

- SwitchOnDeviceXY
- SwitchOffDeviceXY
<br>
<br>
Furthermore, remember, that DBal is big endian. You have to properly marshal your data before transmission and unmarshal them after reception. <br>

**[Functions for that purpose can be found here.](../../../mem_utility/marshaling.h)** Read the doxygen comments in the header for understanding. <br>
A popular BSH method, which is to simply cast structures with data to a byte array, will not work (for parameters bigger than 8 bit), because:<br>

- Many controllers are little endian.
- The data alignment is different for almost each controller.
- The data pointer in DBal receive callbacks is definitely NOT aligned. (It was assumed, that users are able to properly marshal data.)
<br>
<br>

Apart from that, another popular BSH practise must be addressed:<br>
Firing messages with statuses every few milliseconds, even if nothing has changed and no one has requested that information.<br>
This has always been very bad, because it is littering Dbus2 with unnecessary information, limiting its' functioning.<br>
Therefore: <br>

- Send commands only, if you want the server to change something.
- Send queries only, if you really need that information.
- Send events only, if something in your device has changed.
- DO NOT fire the status without anything changing and anybody asking.
<br>
<br>

Moreover, users should not call the sending interface from interrupt routines, as the actions performed inside are extensive.<br>
In case the return value of the sending interface is negative, the internal queue for repeating unresponded messages may be full. <br> 
Therefore, wait for at least DBAL_RESPONSE_TIME_MS before trying again. <br>

## Error Processing
In the following subchapters a more detailed look at the error processing in DBal shall be taken, considering the meaning of the user configuration as well.<br>

### Error Processing In Connection Handling

#### Sending Of Connection Frames (Enabling And Disabling Connection)
DBal driver is notified by Dbus2, whether a connection frame was successfully delivered or not.<br>
<br>
If the frame was not delivered, the action depends on the reported error:<br>

- In case of the error DLL_ACK_NOT_RECEIVED, DBal assumes, that the participant is hibernating. In this case, there are two options:

    - If the message is an "Enable Request", a Dbus2 Wakeup Break is sent and the message transmission is retried after the time DBAL_DBUS_RECOVERY_TIME_MS, which is defined in DBal_cfg.h. If a "WakeupSentRequest" is received from the participant to be woken up,
the retry to send the "Enable Request" is performed earlier, right after the reception of "WakeupSentRequest".
    - In case of other connection messages, the transmission is NOT retried and the connection will be discarded.
- In case of other errors, the transmission of the message is just retried (without sending a Dbus2 Wakeup Break).

The maximum number of retries is defined by DBAL_DBUS_RETRY_MAX in DBal_cfg.h.<br>
<br>
If a connection message could not be delivered after the maximum number of retries, the application is notified about the connection DBAL_COMMSTATE_NOT_READY, if callbacks have been registered for that purpose.<br>
<br>
In case the enable/disable request was delivered, the enable/disable response is expected. If unresponded, the request is repeated after DBAL_RESPONSE_TIME_MS (DBal_cfg.h.) up to DBAL_DBUS_RETRY_MAX times. <br>
<br>

#### Sending Of Connection Frames (Ping)
If a "Ping Request" or "Ping Response" could not be delivered by Dbus2, the procedure is the same as described in the previous [subchapter.](#sending-of-connection-frames-enabling-and-disabling-connection) <br>
<br>
If the Ping mechanism is activated, the client will send its' server a "Ping Request" whenever there has been no communication between the partners 
for the defined pinging time interval. This is for the client to ensure, that the server is still available. <br>
<br>
The server is expected to respond with a "Ping Response" within one second. The result of the pinging is considered positive, if the client either responds 
within the timeout by sending a "Ping Response" (which is done automatically by the DBal driver) or else sends another valid message (e.g. an event).<br>
<br>
If a "Ping Request" is not responded within one second, it will be repeated up to two times. <br>
After three unsuccessful "Ping Requests", the connection will be disabled without informing the communication partner.

#### Receicing Of Connection Frames
DBal checks upon reception: <br>

- whether the communication partner is known and has a valid address.
- whether the length of the frame is correct.
- whether the generic protocol type is correct.
- whether the DBal protocol version is correct.

If any of the points above is not fulfilled, the application will be notified by calls of "DBAL_ntfCorruptConDbus2FrameReceived" and the frame discarded.<br>

You may make the call of the function mentioned above visible, e.g. by lighting an led or by printing **[debug messages.](#debugging-interface)**

### Error Processing For Messages Sent By Application
If a Dbus2 DBal request/response frame is not delivered, the action depends on the dbusdll error code, which is reported by Dbus2:<br>

- If the code is DLL_ACK_NOT_RECEIVED, the connection will be disabled, without informing the communication partner (that is, without sending a connection frame).
- For other error codes, it will be retried to send the frame WITHOUT increasing the sequenceId. If the frame could not be delivered after DBAL_DBUS_RETRY_MAX (DBal_cfg.h) retries, the connection will be disabled, without informing the communication partner (that is, without sending a connection frame).
<br>
<br>
Whenever users are sending commands, queries or events, an acknowledging response (command ack, query ack, event ack) will be expected.
If the according response is not received after DBAL_RESPONSE_TIME_MS (DBal_cfg.h) the unresponded DBal frame will be repeated. 
(The Dbus2 frame will contain an increased sequenceId.) 
If no response has been received after DBAL_DBUS_RETRY_MAX (DBal_cfg.h) retries, the connection will be disabled, without informing the communication partner. <br>
The maximum number of messages, which might be queued for repetition (for each communication partner) is DBAL_MAX_MSGS2REPEAT (DBal_cfg.h). <br>
The maximum size of the payload in messages for repetition is DBAL_MAX_DATALEN_2REPEAT (DBal_cfg.h). <br>
<br>
If the repetition queue is full and the application wants to send another command, query or event, the according sending function of DBal will return a negative return value. The frame will not be sent. <br>
If the application wants to send a command, query or event, which has a payload larger than DBAL_MAX_DATALEN_2REPEAT (DBal_cfg.h), the return value will be negative as well. The frame will not be sent. <br>

### Error Processing For Messages Received By Application
Upon reception of Dbus2 DBal request/response frames, DBal checks: <br>

- whether the communication partner is known.
- whether the length of the frame is correct.
- whether the generic protocol type is correct.

If any of the points above is not fulfilled, the application will be notified by calls of "DBAL_ntfCorruptReqRespDbus2FrameReceived" and the frame discarded.<br>
<br>
Furthermore, the value of the sequenceId is checked. If it is not larger, than the last one, the entire Dbus2 frame will be ignored.
The sequenceId is circulating, which means, that zero comes after UINT8_MAX.<br>
<br>
During the following processing, checks for further inconsistencies take place and any of the functions<br>

- DBAL_ntfUnknownDBalFrameReceived
- DBAL_ntfUnexpectedDBalFrameReceived
- DBAL_ntfCorruptReqRespDbus2FrameReceived

may be called, if necessary. Check the doxygen documention of these functions for details.

You may make the call of the functions above visible, e.g. by lighting an led or by printing **[debug messages.](#debugging-interface)**

### Error Processing In Bookkeeping Of Communication Partners
If "DBAL_init" (and, if needed, "DBALCR_init") HAS NOT correctly been called at the beginning of the application, all Dbus2 DBal connection/request/response frames received will be rejected and the application notified by calls of "DBAL_ntfCorruptConDbus2FrameReceived" or "DBAL_ntfCorruptReqRespDbus2FrameReceived".<br>
Furthermore, all attempts to queue DBal frames for transmission will be rejected.<br>
This is due to the fact, that an unintialized communication partner is unknown.

## Debugging Interface

If you can observe inexplicable errors, when you believe to have configured DBal correctly, 
following interface may be used to print debug messages from the inside of the DBal code: <br>

[DBal Debug Interface](../BshDBus2AppLayer_debug.h) <br>

It is mapped to the following MCU Framework interface: <br>

[Debug](../../../debug) <br>

[Debug config file](../../../debug/cfg_templates/debug_api_config.h) <br>

The debug messages can be printed either via a debug UART or with on-chip-debugging (in Keil MicroVision or SEGGER Ozone).<br>
To activate the DBal debug commands, 
define the macro DBAL_DEBUG in your build process and add the module "debug" to the list "common_components" in your project's makefile.<br>

Now you need to configure and intitialize the MCU Framework interface to lead the messages out of your controller. **[(see)](../../../debug/doc/readme.md)**<br>
<br>
A DBal debug command always prints: <br>

 - the file name.
 - the code line.
 - the message itself.
 - an endline mark.

<br>
As printing messages via UART is negatively impacting the performance of a software, the amount of debug commands in the DBal code is kept at a minimum.
Basically, the initialization phase and error detection are covered.
If needed, you may temporarily increase the amount of debug commands for your own debugging purposes; especially, if you are using the OCD interface. 
If needed, you might use the commands in your own code as well, in order to get a complete picture. <br>
The maximum length of the debug message as such (without file name, code line, end line mark) is 100 bytes, including the parsed parameters. 
If you want to use the debug commands from DBal for debugging your own code as well and need longer messages, 
set DBAL_DBG_MAX_LEN to a higher value inside your build process. <br>
<br>
Note, that while you are using the debug commands, you might need a slightly bigger stack and slightly bigger values of DBAL_RESPONSE_TIME_MS and DBAL_DBUS_RECOVERY_TIME_MS. <br>
<br> 

**DO NOT ACTIVATE THE DEBUG COMMANDS IN THE RELEASE VERSION OF YOUR SOFTWARE!!!**<br>
<br>

## RAM Usage In DBal

DBal describes its' connections by static instances placed in RAM at linktime. These instances contain the logical state of the connection, as well as the data
to be sent, timers, internal and user callbacks.<br>
<br>
If the parameters DBAL_MAX_MSGS2REPEAT and DBAL_MAX_DATALEN_2REPEAT are set to the standard values in DBal_cfg.h, the following is true: <br>
For the main connection to SystemMaster, the instance needs between 300 and 708 bytes, depending on how large the transmit buffer of Dbus2 is (between 50 and 255 bytes). <br>
It will be 4 additional bytes more, if cross connections have been activated. <br>
For cross connections [ecu to ecu communication](#ecu-to-ecu-communication) the estimated size of the instance is between 296 and 500 bytes. <br>
The estimations above are correct for 32 bit controllers and take padding bytes into account. <br>
<br>
DBal has its' Dbus2 transmit table in RAM, which contains 3 entries for the connection to SystemMaster and another 3 entries for [ecu to ecu communication](#ecu-to-ecu-communication), if it is activated. <br>
<br>
The maximum estimated stack usage in DBal is ordinarily around 20 bytes.<br>
When debug messages are activated, the maximum stack usage will increase by 100 bytes, or whatever other value DBAL_DBG_MAX_LEN has been set to by the user.

## Quality Assurance For DBal Driver In MCU Framework
DBal has been intensely tested in three ways:<br>

- in a very detailed integration test of the [example application](https://scr.bsh-sdd.com/projects/SYMANA/repos/mcu_samples/).
- in a unittest, which completely covers all logical scenarios, including all sorts of possible errors.
- in a [stress test](https://github-bshg.boschdevcloud.com/CommonComponents/dbalTest) of the ecu to ecu communication.

Furthermore, the entire code has been statically checked with lint, lintplus and axivion.<br>
If DBal debug commands are inactive, all three code checkers give positive results.<br>
In case of activated DBal debug commands: <br>

- the lint/lintplus check of all files will pass.
- the axivion check of all c files in DBal will fail. Axivion does not like the macro used not to print the absolute path to the c file, but just a relative one.

## What, If I Believe To Have Found A Bug?
Please report the issue to the address PED-support@bshg.com, giving a full description of your issue and the information, whom to contact.<br>
Depending on who is present/available, the issue will be processed either by the authors of the DBal code (Anton.ZolnaAn@bshg.com, Ondrej.Veres@bshg.com) or by the Dbus support team. Each issue will be processed as a PED-Support ticket.<br>
This procedure is important, in order to document all possible issues in a central place and to organize needed bugfixes in LTS branches of the MCU Framework.

## Where Do I Ask Questions, Which Remain After Reading This Docu, Doxy Comments And DBal Specification?
If the questions concern the functioning of the MCU Framework implementation of DBal, contact Anton.ZolnaAn@bshg.com or Ondrej.Veres@bshg.com.<br>

If the questions concern the overall concept behind DBal communication, the best contacts are:<br>
Sebastian.Deller@bshg.com, Alexander.Arnoldi@bshg.com, Ralf.Hochhausen@bshg.com and Alexander.Kiefl@BSHG.COM.

## Things We Will Do In Future

- Dynamic setting of your own node address (main- and subnode) at the start of runtime.
- RTOS support

## Release
Changes are documented on the [Dbus2 Release Page](https://wiki.bsh-sdd.com/x/dZ07Pg), as well as in the MCU Framework's changelog.
