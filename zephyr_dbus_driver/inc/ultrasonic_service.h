#ifndef ULTRASONIC_SERVICE_H__
#define ULTRASONIC_SERVICE_H__

#include <stdint.h>

#include "dbus_driver_public.h"

#define ULTRASONIC_REG_BASE           0x5100u
#define ULTRASONIC_REG_STRIDE         0x04u
#define ULTRASONIC_REG_DISTANCE_OFF   0x00u
#define ULTRASONIC_MAX_SENSOR_INDEX   0u

enum DBC_Error ultrasonic_service_get_distance_mm(uint8_t sensor_index, uint16_t *distance_mm);

#endif
