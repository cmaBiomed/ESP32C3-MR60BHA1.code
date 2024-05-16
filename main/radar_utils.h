/*
@author Carlos Madrid Espinosa
@date 2024/05/10
*/

#ifndef _RADAR_UTILS_H__
#define _RADAR_UTILS_H__

#include "Arduino.h"
#include "60ghzbreathheart.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>
// #include <ArduinoJson.hpp> // i dont know if I should include the Json Methods in this file

// UART conection with the sensor
#define ESP_RX_SENSOR_TX 4 // Pin conected to the sensors Tx pin
#define ESP_TX_SENSOR_RX GPIO_NUM_21 // Pin conected to the sensors Rx pin
#define UART_BAUD_RATE 115200        // Baud rate for the sensor's UART conection

// Times for different things (in seconds)
#define SLEEP_TIME      10   // Seconds that the system will sleep
#define MEASURE_TIME    20   // Maximun time (in seconds) for masuring heart rate and breath rate
#define SAMPLE_TIME     3   // Maximun time (in seconds) for taking a sample of heart rate and breath rate
#define DETECTION_TIME  10   // Maximun time (in seconds) for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

// Storage struct used to record the reported vital sings
struct recorded_vital_sings {
    float sample_time;                  // Time of the taken measure
    float mean_sample_heart_rate;       // Mean heart rate at that sample
    float mean_sample_breath_rate;      // Mean breart rate at that sample
};

extern size_t data_size;            // Aumount of samples stored

void sensor_init();
float person_detec();
recorded_vital_sings *vital_sings_measure();

#endif /*_RADAR_UTILS_H__*/