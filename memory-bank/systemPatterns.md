# System Patterns

## Current System Architecture
The DBus driver follows a layered architecture:
- **DBAL (DBus Application Layer):** Handles high-level application-specific communication, connection management, and message repetition.
- **BAL (Bus Application Layer):** Manages message distribution, transmission scheduling, and interfaces with the presentation and data link layers.
- **DBPL (DBus Presentation Layer):** Processes predefined service messages (e.g., read/write requests, identity), manages memory modules, and handles mode transitions.
- **DLL (Data Link Layer):** Responsible for low-level message framing, CRC, and physical transmission/reception over the bus (e.g., UART, DBusCAN).
The system uses an RTOS interface (DBR) for event-driven task management and a locking mechanism (DBLK) for bus access control.

## Communication Patterns
- **Request-Response:** Commands and queries are sent, expecting an acknowledgment or response.
- **Event-driven:** Events are broadcast or sent to specific partners, with acknowledgments.
- **Connection-oriented:** Explicit connection enable/disable and ping messages are used for communication partners.
- **Message Retransmission:** Mechanisms are in place to retransmit messages if no acknowledgment or response is received.

## Data Flow
- **Application to Driver:** Application modules prepare DBus messages and pass them to DBAL/BAL for transmission.
- **Driver to Bus:** Messages are framed by DLL, potentially via a DBusCAN driver, and sent over the physical bus.
- **Bus to Driver:** Incoming messages are received by DLL, de-framed, and passed up to DBPL/BAL/DBAL for processing.
- **Driver to Application:** Processed messages (responses, events) are dispatched to registered application callbacks.
- **Configuration Data:** Passed during initialization to configure node addresses, baud rates, and buffer lengths.

## Error Handling
- **CRC Checking:** Messages include a 16-bit CRC for integrity verification at the DLL.
- **Acknowledgment (ACK/NACK):** The DLL handles positive and negative acknowledgments from communication partners.
- **Retries:** Messages are retransmitted a configurable number of times upon missing or negative acknowledgments.
- **Timeouts:** Interbyte and acknowledgment timeouts are used to detect communication failures.
- **Error Notifications:** Higher layers are notified of corrupt frames, unexpected messages, or non-deliverable messages via callbacks.
- **Bus Lock:** The DBLK module prevents simultaneous conflicting operations on the bus.

## Integration Points
- **Hardware Interface:** The DLL and DBusCAN Driver interface directly with UART/CAN peripherals (via DBM and MCALs).
- **RTOS Integration:** The DBR module provides an abstraction layer for RTOS services (tasks, event flags).
- **Application API:** DBAL and DBPL expose APIs for application modules to interact with the DBus.
- **Configuration:** `DBal_cfg.c/.h` defines configurable parameters for the DBAL.
- **Build System:** Integration with Makefiles for compilation and linking.

## Zephyr RTOS Integration Considerations
- How will existing communication patterns map to Zephyr's IPC mechanisms (message queues, pipes, semaphores, mutexes)?
- How will error handling and logging be adapted to Zephyr's framework?
- What changes are needed in the build system to incorporate Zephyr's Kconfig and Device Tree?
- How will the current layered architecture be mapped to Zephyr's driver model and subsystems?
- How will the DBR and DBLK functionalities be implemented using Zephyr's kernel services?