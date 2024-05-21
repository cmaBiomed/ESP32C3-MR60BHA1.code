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

// Initalization of the UART conection and the sensor
HardwareSerial Sensor_Serial(1);
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

/*
* Start of the UART conection asigned to the sensor
* @todo fix this
*/
void sensor_init() {
    Sensor_Serial.begin(115200, SERIAL_8N1, ESP_RX_SENSOR_TX, ESP_TX_SENSOR_RX);
    while(!Sensor_Serial) {
        delay(200);
    }
}

/*
* Function that determines wether a person is located within the vacinity of the sensor. It uses the distance 
* functionalitie of the sensor, where we determine that if the sensor reports a distance during the stablished 
* time, then we can say that a person has been detected. This is an estimation and can fail in some scenarios.
*/
float person_detect() {
    float person_distance = 0.0f;
    unsigned long Start_Time = millis();
    bool measured_distance = false;
    while (millis() - Start_Time < (unsigned long) DETECTION_TIME*mS_S && !measured_distance) {  
        radar.HumanExis_Func();
        if (radar.sensor_report ==  DISVAL && radar.distance > 0.4f) {
            person_distance = radar.distance;
            measured_distance = true;
        }
    }
    radar.reset_func();
    return person_distance;
}

static const size_t INITIAL_CAPACITY = 10; // Amount of samples that can be stored initially
static const size_t MAX_CAPACITY = 100;    // Mximun amount of samples

size_t data_size = 0;

/*
* Function that for a determined time scans a persons's vital sings (heart and breath rate). The sensor gives 
* this information in 5 reports during 3 seconds, so we take a mean of the values reported during this 3 seconds
* (the sample time can be changed) and record the time so that we can have a relation between time and measures.
* This information is stored in an storage struct, and returned as an array. 
*/
recorded_vital_sings *vital_sings_measure() {
    
    recorded_vital_sings *vitals_array = (recorded_vital_sings*)malloc(INITIAL_CAPACITY * sizeof(recorded_vital_sings));
    unsigned long start_time = millis(), sample_time = millis();
    float sum_HEART_RATE = 0, sum_BREATH_RATE = 0;
    int heart_rate_points = 0, breath_rate_points = 0;

    while (millis() - start_time < (unsigned long) MEASURE_TIME*mS_S && data_size < MAX_CAPACITY) {
        radar.Breath_Heart();
        switch(radar.sensor_report) {
            case HEARTRATEVAL:
                sum_HEART_RATE += radar.heart_rate;
                heart_rate_points++;
                break;
            case BREATHVAL:
                sum_BREATH_RATE += radar.breath_rate;
                breath_rate_points++;
                break;
            default:
                break;  
        } 
        if (millis() - sample_time > (unsigned long) SAMPLE_TIME*mS_S) {
            vitals_array = (data_size % INITIAL_CAPACITY == 0) ?
            (recorded_vital_sings*)realloc(vitals_array, (
                (MAX_CAPACITY-data_size >= INITIAL_CAPACITY) ?
                    data_size + INITIAL_CAPACITY 
                    : MAX_CAPACITY-data_size
                ) * sizeof(recorded_vital_sings)) 
            : vitals_array;
            if (vitals_array != nullptr) {
                heart_rate_points = (heart_rate_points == 0) ? 1 : heart_rate_points;
                breath_rate_points = (breath_rate_points == 0) ? 1 : breath_rate_points;
                vitals_array[data_size].sample_time = (float)(millis()-start_time)/1000;
                vitals_array[data_size].mean_sample_heart_rate = sum_HEART_RATE/(float)heart_rate_points;
                vitals_array[data_size].mean_sample_breath_rate = sum_BREATH_RATE/(float)breath_rate_points;
                data_size++;
            } else data_size = MAX_CAPACITY; // if we get a null pointer we shouldn't keep readig values
            // rest all values
            sum_HEART_RATE = 0;
            sum_BREATH_RATE = 0;
            heart_rate_points = 0;
            breath_rate_points = 0;
            sample_time = millis();
        }
    }
    radar.reset_func();
    return vitals_array;
}

#endif /*_RADAR_UTILS__*/