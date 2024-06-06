/** 
 * @author Carlos Madrid Espinosa
 * @date 2024/05/10
 * @brief Colection of different utilities I have developed to control the MR60BHA1 sensor from seed studio.
 * It contains functionalities like initializing the sensors UART conection, detecting people and obtaining
 * their position values and reading a persons heart and breath rate over an specified time.
*/

#include "radar_utils.h"
#include "Arduino.h"
#include "60ghzbreathheart.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// UART conection with the sensor
#define ESP_RX_SENSOR_TX 4 // Pin conected to the sensors Tx pin
#define ESP_TX_SENSOR_RX 5 // Pin conected to the sensors Rx pin
#define UART_BAUD_RATE 115200        // Baud rate for the sensor's UART conection

// Times for different things (in seconds)
#define SLEEP_TIME      1    // Seconds that the system will sleep
#define MEASURE_TIME    20   // Maximun time (in seconds) for masuring heart rate and breath rate
#define SAMPLE_TIME     2    // Maximun time (in seconds) for taking a sample of heart rate and breath rate
#define DETECTION_TIME  10   // Maximun time (in seconds) for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

// Initalization of the UART conection and the sensor
HardwareSerial Sensor_Serial(1);
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

/**
 * Start of the UART conection asigned to the sensor
*/
void sensor_init() {
    Sensor_Serial.begin(115200, SERIAL_8N1, ESP_RX_SENSOR_TX, ESP_TX_SENSOR_RX);
    while(!Sensor_Serial) {
        delay(200);
    }
}

char * person_detect() {
    unsigned int Start_Time = millis();
    bool measured_distance = false;

    while (millis() - Start_Time < (unsigned long) DETECTION_TIME*mS_S && !measured_distance) {  
        radar.HumanExis_Func();
        if (radar.sensor_report ==  DISVAL && radar.distance > 0.4f) {
            values.distance = radar.distance;
            values.time_stamp = (millis())/(unsigned int)mS_S;
            measured_distance = true;
        }
    }
    radar.reset_func();
    return ;
}


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
                vitals_array[data_size].sample_time = (float)millis()/1000;
                vitals_array[data_size].mean_sample_heart_rate = (float)sum_HEART_RATE/heart_rate_points;
                vitals_array[data_size].mean_sample_breath_rate = (float)sum_BREATH_RATE/breath_rate_points;
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