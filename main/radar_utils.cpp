/*
@author Carlos Madrid Espinosa
@date 2024/05/10
@brief Colection of different utilities I have developed to control the MR60BHA1 sensor from seed studio.
* It contains functionalities like initializing the sensors UART conection, detecting people and obtaining
* their position values, reading a persons heart and breath rate over an specified time and functions that 
* packs all this information into .JSON files. 
*/

#ifndef _RADAR_UTILS__
#define _RADAR_UTILS__

#include "radar_utils.h"
#include "Arduino.h"
#include "60ghzbreathheart.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>
// #include <ArduinoJson.hpp>

// initalization of the UART conection and the sensor
HardwareSerial Sensor_Serial(1);
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

recorded_vital_sings* vitals_array = nullptr;
size_t data_size = 0;
const size_t INITIAL_CAPACITY = 10;
const size_t MAX_CAPACITY = 100;

/*
* Start of the UART conection asigned to the sensor
*/
void sensor_init() {
    Sensor_Serial.begin(115200, SERIAL_8N1, Rx, Tx);
    while(!Sensor_Serial) {
        delay(200);
    }
}

float person_direction[3]; 
float person_distance;

/*
* Function that determines wether a person is located within the vacinity of the sensor.
* It uses the distance and direcction functionalities of the sensor, where we determine that if the sensor
* reports both of those values during the stablished time, then we can say that a person has been detected. 
* This is an estimation and can fail in some scenarios.
*/
bool person_detec() {
    unsigned long Start_Time = millis();
    bool measured_distance = false;
    bool measured_direction = false;
    while (millis() - Start_Time < (unsigned long) DETECTION_TIME*mS_S  && !(measured_distance && measured_direction)) { 
        radar.HumanExis_Func();
        if (radar.sensor_report != 0x00) {
            switch (radar.sensor_report) {
                case DISVAL:
                    person_distance = radar.distance;
                    measured_distance = true;
                    break;
                case DIREVAL:
                    person_direction[0] = radar.Dir_x;
                    person_direction[1] = radar.Dir_y;
                    person_direction[2] = radar.Dir_z;
                    measured_direction = true;
                    break;
                default:
                    break;
            }
        }
    }
    radar.reset_func();
    return measured_distance && measured_direction;
}

/*
* Function that for a determined time scans the vital sings (heart and breath rate) forma a person.
* The sensor gives this information in 5 reports during 3 seconds, so we take a mean of the values reported
* during this 3 seconds and record the time so that we can have a relation between time and measures.
* @todo find a way to store the measures so that they dont have to be printed on the serial monitor, istead
* they are added to a json file that is the payload sent trough mqtt.
*/
void vital_sings_measure() {
    unsigned long start_time = millis();
    unsigned long sample_time = millis();
    float sum_HEART_RATE = 0;
    float sum_BREATH_RATE = 0;
    int heart_rate_points = 0;
    int breath_rate_points = 0;
    allocate_vital_sings_array();
    while (millis() - start_time < (unsigned long) MEASURE_TIME*mS_S) {
        if (millis() - sample_time < (unsigned long) SAMPLE_TIME*mS_S) {
            radar.Breath_Heart();
            if(radar.sensor_report != 0x00){
                switch(radar.sensor_report) {
                    case HEARTRATEVAL:
                        sum_HEART_RATE += radar.heart_rate;
                        heart_rate_points++;
                        break;
                    case BREATHVAL:
                        sum_BREATH_RATE += radar.breath_rate;
                        breath_rate_points++;
                        break;
                }
            }
        } 
        else {
            // check if 0 so that i dont get an error when calculating the mean.
            heart_rate_points = (heart_rate_points == 0) ? 1 : heart_rate_points;
            breath_rate_points = (breath_rate_points == 0) ? 1 : breath_rate_points;
            // We add the recorded values to the vitals array
            add_vitals_measure((float)(millis()-start_time)/1000, sum_HEART_RATE/(float)heart_rate_points, sum_BREATH_RATE/(float)breath_rate_points);
            // rest all values
            sum_HEART_RATE = 0;
            sum_BREATH_RATE = 0;
            heart_rate_points = 0;
            breath_rate_points = 0;
            sample_time = millis(); 
        }
    }
    radar.reset_func();
}



// Initializes the vital sings array allocating the initial memory required.
void allocate_vital_sings_array() {
    vitals_array = (recorded_vital_sings*)malloc(INITIAL_CAPACITY * sizeof(recorded_vital_sings));
    if (vitals_array == nullptr) return;
}

/*
* Add new values to the recorded vital sings array
* @param float:sample_time  Time at wich the values were recorded
* @param float:recorded_heart_rate  The mean recorded heart rate
* @param float:recorded_breath_rate  The mean recorded breath rate
*/
void add_vitals_measure(float sample_time, float recorded_heart_rate, float recorded_breath_rate) {
    if (data_size >= MAX_CAPACITY) return;
    if (data_size % INITIAL_CAPACITY == 0) {
        vitals_array = (recorded_vital_sings*)realloc(vitals_array, (data_size + INITIAL_CAPACITY) * sizeof(recorded_vital_sings));
        if (vitals_array == nullptr) return;
    }
    vitals_array[data_size].smaple_time = sample_time;
    vitals_array[data_size].mean_sample_heart_rate = recorded_heart_rate;
    vitals_array[data_size].mean_sample_breath_rate = recorded_breath_rate;
    data_size++;
}

// @todo JSON things

#endif /*_RADAR_UTILS__*/