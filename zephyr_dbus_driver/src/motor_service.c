#include "motor_service.h"

#include "dbus_app_layer.h"

#define MOTOR0_ENABLE_ADDR   0x5000u
#define MOTOR0_SPEED_ADDR    0x5004u
#define MOTOR0_FEEDBACK_ADDR 0x5008u
#define MOTOR0_STATUS_ADDR   0x500Cu

#define MOTOR_REG_STRIDE     0x10u

static bool motor_service_index_valid(uint8_t motor_index)
{
    return motor_index <= MOTOR_SERVICE_MAX_INDEX;
}

static enum DBC_RegAddr motor_service_reg_addr(uint8_t motor_index, uint16_t base_addr)
{
    return (enum DBC_RegAddr)(base_addr + ((uint16_t)motor_index * MOTOR_REG_STRIDE));
}

bool motor_service_encode_enable_payload(uint8_t motor_index, bool enable, struct motor_enable_payload *payload)
{
    if ((payload == NULL) || !motor_service_index_valid(motor_index)) {
        return false;
    }

    payload->motor_index = motor_index;
    payload->enable = enable ? 1u : 0u;
    return true;
}

bool motor_service_encode_speed_payload(uint8_t motor_index, int32_t speed_setpoint, struct motor_speed_payload *payload)
{
    if ((payload == NULL) || !motor_service_index_valid(motor_index)) {
        return false;
    }

    if (speed_setpoint > MOTOR_SERVICE_SPEED_ABS_MAX) {
        speed_setpoint = MOTOR_SERVICE_SPEED_ABS_MAX;
    } else if (speed_setpoint < -MOTOR_SERVICE_SPEED_ABS_MAX) {
        speed_setpoint = -MOTOR_SERVICE_SPEED_ABS_MAX;
    }

    payload->motor_index = motor_index;
    payload->speed_setpoint = speed_setpoint;
    return true;
}

bool motor_service_encode_index_payload(uint8_t motor_index, struct motor_index_payload *payload)
{
    if ((payload == NULL) || !motor_service_index_valid(motor_index)) {
        return false;
    }

    payload->motor_index = motor_index;
    return true;
}

enum DBC_Error motor_service_set_enable(uint8_t motor_index, bool enable)
{
    struct motor_enable_payload payload;
    enum DBC_Error reg_err;

    if (!motor_service_encode_enable_payload(motor_index, enable, &payload)) {
        return DBC_ERROR;
    }

    if (!dbal_send_event(MOTOR_SERVICE_ID,
                         MOTOR_CMD_SET_ENABLE,
                         (const uint8_t *)&payload,
                         sizeof(payload))) {
        return DBC_ERROR;
    }

    /* Keep DBAL path active, but mirror to legacy register interface to
     * guarantee deterministic motor state while DBAL wire parsing matures. */
    reg_err = DBCDRV_writeReg32(motor_service_reg_addr(payload.motor_index, MOTOR0_ENABLE_ADDR),
                                payload.enable ? 1u : 0u);
    if (reg_err != DBC_OK) {
        return reg_err;
    }

    return DBC_OK;
}

enum DBC_Error motor_service_set_speed(uint8_t motor_index, int32_t speed_setpoint)
{
    struct motor_speed_payload payload;
    uint8_t wire_payload[5];
    enum DBC_Error reg_err;

    if (!motor_service_encode_speed_payload(motor_index, speed_setpoint, &payload)) {
        return DBC_ERROR;
    }

    wire_payload[0] = payload.motor_index;
    wire_payload[1] = (uint8_t)((uint32_t)payload.speed_setpoint);
    wire_payload[2] = (uint8_t)((uint32_t)payload.speed_setpoint >> 8);
    wire_payload[3] = (uint8_t)((uint32_t)payload.speed_setpoint >> 16);
    wire_payload[4] = (uint8_t)((uint32_t)payload.speed_setpoint >> 24);

    if (!dbal_send_event(MOTOR_SERVICE_ID,
                         MOTOR_CMD_SET_SPEED,
                         wire_payload,
                         sizeof(wire_payload))) {
        return DBC_ERROR;
    }

    reg_err = DBCDRV_writeReg32(motor_service_reg_addr(payload.motor_index, MOTOR0_SPEED_ADDR),
                                (uint32_t)payload.speed_setpoint);
    if (reg_err != DBC_OK) {
        return reg_err;
    }

    return DBC_OK;
}

enum DBC_Error motor_service_get_feedback(uint8_t motor_index, int32_t *speed_feedback)
{
    struct motor_index_payload payload;
    uint32_t raw_feedback = 0;
    enum DBC_Error err;

    if ((speed_feedback == NULL) || !motor_service_encode_index_payload(motor_index, &payload)) {
        return DBC_ERROR;
    }

    err = DBCDRV_readReg32(motor_service_reg_addr(payload.motor_index, MOTOR0_FEEDBACK_ADDR), &raw_feedback);
    if (err != DBC_OK) {
        return err;
    }

    *speed_feedback = (int32_t)raw_feedback;
    return DBC_OK;
}

enum DBC_Error motor_service_get_status(uint8_t motor_index, uint32_t *status)
{
    struct motor_index_payload payload;

    if ((status == NULL) || !motor_service_encode_index_payload(motor_index, &payload)) {
        return DBC_ERROR;
    }

    return DBCDRV_readReg32(motor_service_reg_addr(payload.motor_index, MOTOR0_STATUS_ADDR), status);
}