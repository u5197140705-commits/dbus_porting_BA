#include "ultrasonic_service.h"

static bool ultrasonic_service_index_valid(uint8_t sensor_index)
{
    return sensor_index <= ULTRASONIC_MAX_SENSOR_INDEX;
}

static enum DBC_RegAddr ultrasonic_service_reg_addr(uint8_t sensor_index, uint16_t base_addr)
{
    return (enum DBC_RegAddr)(base_addr + ((uint16_t)sensor_index * ULTRASONIC_REG_STRIDE));
}

enum DBC_Error ultrasonic_service_get_distance_mm(uint8_t sensor_index, uint16_t *distance_mm)
{
    uint32_t raw_value = 0u;
    enum DBC_Error err;

    if (distance_mm == NULL || !ultrasonic_service_index_valid(sensor_index)) {
        return DBC_ERROR;
    }

    err = DBCDRV_readReg32(ultrasonic_service_reg_addr(sensor_index,
                                                       (uint16_t)(ULTRASONIC_REG_BASE + ULTRASONIC_REG_DISTANCE_OFF)),
                           &raw_value);
    if (err != DBC_OK) {
        return err;
    }

    *distance_mm = (uint16_t)(raw_value & 0xFFFFu);
    return DBC_OK;
}
