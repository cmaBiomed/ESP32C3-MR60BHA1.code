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

// UART conection with the sensor
#define Rx 18 // Pin conected to the sensors Tx pin
#define Tx 19 // Pin conected to the sensors Rx pin

// Times for different things (in seconds)
#define SLEEP_TIME      5   // Seconds that the system will sleep
#define MEASURE_TIME    20  // Maximun time (in seconds) for masuring heart rate and breath rate
#define SAMPLE_TIME     3  // Maximun time (in seconds) for taking a sample of heart rate and breath rate
#define DETECTION_TIME  20  // Maximun time (in seconds) for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

// Positional values from the detected person. Termporally here, the idea is to return the jsop file instead
extern float person_direction[3]; 
extern float person_distance;

// Vital sing values from the recorded person
struct recorded_vital_sings {
    float smaple_time;
    float mean_sample_heart_rate;
    float mean_sample_breath_rate;
};

extern recorded_vital_sings* vitals_array;
extern size_t data_size;
extern const size_t INITIAL_CAPACITY;
extern const size_t MAX_CAPACITY;

void sensor_init();
bool person_detec();
void vital_sings_measure();
void allocate_vital_sings_array();
void add_vitals_measure(float sample_time, float recorded_heart_rate, float recorded_breath_rate);


#endif /*_RADAR_UTILS_H__*/