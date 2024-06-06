/**
 * @author Carlos Madrid Espinosa
 * @date 2024/05/10
*/

#ifndef _RADAR_UTILS_H__
#define _RADAR_UTILS_H__

#include "Arduino.h"
#include "60ghzbreathheart.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// Scale factor
#define mS_S    1000    // Mili seconds to seconds
#define uS_S    1000000 // Micro seconds to seconds

extern bool person_detected;

void sensor_init();
const char * person_detect();
const char * vital_sings_measure();

#endif /*_RADAR_UTILS_H__*/