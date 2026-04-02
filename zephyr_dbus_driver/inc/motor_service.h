#ifndef MOTOR_SERVICE_H__
#define MOTOR_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>

#include "dbus_driver_public.h"

#define MOTOR_SERVICE_ID                  0x7100u
#define MOTOR_CMD_SET_ENABLE              0x0001u
#define MOTOR_CMD_SET_SPEED               0x0002u
#define MOTOR_CMD_GET_FEEDBACK            0x0003u
#define MOTOR_CMD_GET_STATUS              0x0004u

#define MOTOR_SERVICE_MAX_INDEX           3u
#define MOTOR_SERVICE_SPEED_ABS_MAX       1200

struct motor_enable_payload {
    uint8_t motor_index;
    uint8_t enable;
};

struct motor_speed_payload {
    uint8_t motor_index;
    int32_t speed_setpoint;
};

struct motor_index_payload {
    uint8_t motor_index;
};

struct motor_feedback_payload {
    uint8_t motor_index;
    int32_t speed_feedback;
};

struct motor_status_payload {
    uint8_t motor_index;
    uint32_t status;
};

bool motor_service_encode_enable_payload(uint8_t motor_index, bool enable, struct motor_enable_payload *payload);
bool motor_service_encode_speed_payload(uint8_t motor_index, int32_t speed_setpoint, struct motor_speed_payload *payload);
bool motor_service_encode_index_payload(uint8_t motor_index, struct motor_index_payload *payload);

enum DBC_Error motor_service_set_enable(uint8_t motor_index, bool enable);
enum DBC_Error motor_service_set_speed(uint8_t motor_index, int32_t speed_setpoint);
enum DBC_Error motor_service_get_feedback(uint8_t motor_index, int32_t *speed_feedback);
enum DBC_Error motor_service_get_status(uint8_t motor_index, uint32_t *status);

#endif