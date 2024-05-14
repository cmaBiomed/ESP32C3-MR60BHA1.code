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

float person_direction [3]; 
float person_distance;

// Storage struct used to record the reported vital sings
struct recorded_vital_sings {
    float sample_time;
    float mean_sample_heart_rate;
    float mean_sample_breath_rate;
};

// Utilities for allocating and realocating memory used to store the reported vital sings
recorded_vital_sings* vitals_array = nullptr;
size_t data_size = 0;
const size_t INITIAL_CAPACITY = 10;
const size_t MAX_CAPACITY = 100;

/*
* Stablishes the comunication mode.
* @param int:mode  0: serial; 1: MQTT 
*/
void set_Comunication_Mode(int mode) {
    USE_MQTT = (mode == 1); 
} 

/*
* Start of the UART conection asigned to the sensor
*/
void sensor_init() {
    Sensor_Serial.begin(115200, SERIAL_8N1, ESP_RX_SENSOR_TX, ESP_Tx_SENSOR_RX);
    while(!Sensor_Serial) {
        delay(200);
    }
}

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
                    measured_distance = person_distance > 0.4f; // the efective range starts at 0.4m
                    break;
                /* This can be skipped as it is not that effective
                case DIREVAL: 
                    person_direction[0] = radar.Dir_x;
                    person_direction[1] = radar.Dir_y;
                    person_direction[2] = radar.Dir_z;
                    measured_direction = true;
                    break;
                */
                default:
                    break;
            }
        }
    }
    radar.reset_func();
    if (USE_MQTT) {
        // IDK
    }
    Output_serial.println("Person detected at: ");
    Output_serial.print(person_distance);
    Output_serial.println("m.");
    return measured_distance && measured_direction;
}

/*
* Function that for a determined time scans the vital sings (heart and breath rate) forma a person.
* The sensor gives this information in 5 reports during 3 seconds, so we take a mean of the values reported
* during this 3 seconds (the sample time can be cahnged) and record the time so that we can have a relation 
* between time and measures.
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
            add_vitals_measure((float)(millis()-start_time)/1000, 
                                sum_HEART_RATE/(float)heart_rate_points, 
                                sum_BREATH_RATE/(float)breath_rate_points);
            // rest all values
            sum_HEART_RATE = 0;
            sum_BREATH_RATE = 0;
            heart_rate_points = 0;
            breath_rate_points = 0;
            sample_time = millis();
        }
    }
    radar.reset_func();
    if (USE_MQTT) {
        // IDK
    }
    print_vitals();
    free(vitals_array);
}

void print_vitals() {
    Output_serial.println("t \t | \t Hr \t , \t Br \t");
    Output_serial.println("---------------------"); 
    for(int i = 0; i<data_size-1; i++) {
        Output_serial.print(vitals_array[i].sample_time);
        Output_serial.print(" \t | \t ");
        Output_serial.print(vitals_array[i].mean_sample_breath_rate);
        Output_serial.print(" \t , \t ");
        Output_serial.print(vitals_array[i].mean_sample_breath_rate);
        Output_serial.println(" \t");
    }
}

/* 
* Initializes the vital sings array allocating the initial memory required. Initially we can store up to 10 
* samples. If we cant allocate it returns. This avoids posible stack overflows.
*/
void allocate_vital_sings_array() {
    vitals_array = (recorded_vital_sings*)malloc(INITIAL_CAPACITY * sizeof(recorded_vital_sings));
    if (vitals_array == nullptr) return;
}

/*
* Add new values to the recorded vital sings array. It reallocates memory to the vitals array. To avoid 
* endless memory ocupation, there is a maximun capacity to the amount of samples taken. The initial size 
* is 10 so any time the initial capacity is reached, we add space for 10 more samples. Untill we reach 
* the maximun capacity of 100 samples.
* @param float:sample_time          Time at wich the values were recorded
* @param float:recorded_heart_rate  The mean recorded heart rate
* @param float:recorded_breath_rate The mean recorded breath rate
*/
void add_vitals_measure(float sample_time, float recorded_heart_rate, float recorded_breath_rate) {
    if (data_size >= MAX_CAPACITY) return;
    if (data_size % INITIAL_CAPACITY == 0) {
        vitals_array = (recorded_vital_sings*)realloc(vitals_array, (data_size + INITIAL_CAPACITY) * sizeof(recorded_vital_sings));
        if (vitals_array == nullptr) return;
    }
    vitals_array[data_size].sample_time = sample_time;
    vitals_array[data_size].mean_sample_heart_rate = recorded_heart_rate;
    vitals_array[data_size].mean_sample_breath_rate = recorded_breath_rate;
    data_size++;
}

// @todo Seria coms things

/*
* We start the UART port used to send the values to the outside
*/

// @todo JSON things

#endif /*_RADAR_UTILS__*/