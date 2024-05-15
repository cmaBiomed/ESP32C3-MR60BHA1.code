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
#define ESP_RX_SENSOR_TX 18 // Pin conected to the sensors Tx pin
#define ESP_TX_SENSOR_RX 19 // Pin conected to the sensors Rx pin
/* This will be the final ones but i still don't know how to set them up to do it ChatGPT and the esp IDF 
* documentation are helping tho
#define ESP_RX_SENSOR_TX GPIO_NUM_20 // Pin conected to the sensors Tx pin
#define ESP_TX_SENSOR_RX GPIO_NUM_21 // Pin conected to the sensors Rx pin
*/

// Times for different things (in seconds)
#define SLEEP_TIME      10   // Seconds that the system will sleep
#define MEASURE_TIME    21   // Maximun time (in seconds) for masuring heart rate and breath rate
#define SAMPLE_TIME     10   // Maximun time (in seconds) for taking a sample of heart rate and breath rate
#define DETECTION_TIME  10   // Maximun time (in seconds) for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

extern unsigned int output; 

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
void add_vitals_measure(recorded_vital_sings *vitals_array, float sample_time, float recorded_heart_rate, float recorded_breath_rate);

// float raw_output();

#endif /*_RADAR_UTILS_H__*/