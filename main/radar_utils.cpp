/** 
 * @author Carlos Madrid Espinosa
 * @date 2024/05/10
 * @brief Colection of different utilities I have developed to control the MR60BHA1 sensor from seed studio.
 * It contains functionalities like initializing the sensors UART conection, detecting people and obtaining
 * their position values and reading a persons heart and breath rate over an specified time.
*/

#include "radar_utils.h"

// UART conection with the sensor
#define ESP_RX_SENSOR_TX 4      // Pin conected to the sensors Tx pin
#define ESP_TX_SENSOR_RX 5      // Pin conected to the sensors Rx pin
#define UART_BAUD_RATE 115200   // Baud rate for the sensor's UART conection

// Times for different things (in seconds)
#define MEASURE_TIME    10   // Maximun time (in seconds) for masuring heart rate and breath rate
#define SAMPLE_TIME     3    // Maximun time (in seconds) for taking a sample of heart rate and breath rate
#define DETECTION_TIME  10   // Maximun time (in seconds) for searching people in the person detection mode

// Initalization of the UART conection and the sensor
HardwareSerial Sensor_Serial(1);
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

// Check wether a person has been detected
bool person_detected = false;

/**
 * Start of the UART conection asigned to the sensor
*/
void sensor_init() {
    Sensor_Serial.begin(115200, SERIAL_8N1, ESP_RX_SENSOR_TX, ESP_TX_SENSOR_RX);
    while(!Sensor_Serial) {
        delay(200);
    }
}

/**
 * We determine that a person has been detected when we get a distance report from the sensor repot. 
 * In that case we record that distance and the time it was recorded at, add them to a JSON file, 
 * serialize it and retun it.
*/
const char * person_detect() {
    StaticJsonDocument<32> detect_doc;
    unsigned int Start_Time = millis();
    bool measured_distance = false;
    detect_doc["distance"] = -1.0f;
    detect_doc["timestamp"] = millis();
    while (millis() - Start_Time < (unsigned long) DETECTION_TIME*mS_S && !measured_distance) {  
        radar.HumanExis_Func();
        if (radar.sensor_report == DISVAL && radar.distance > 0.4f) {
            detect_doc["distance"] = radar.distance;
            detect_doc["timestamp"] = millis();
            measured_distance = true;
        }
    }
    person_detected = measured_distance;
    radar.reset_func();
    String serialized_detect;
    serializeJson(detect_doc, serialized_detect);
    return serialized_detect.c_str();
}

/**
 * We read the heart and breath rate of a person over an spcecified time. We read the values over a 
 * specified sample time, and average them to reduce the possible variations. Then we add the saple 
 * mean to a json file as a json array. Finally we serialize the complete file and return it.
*/
const char * vital_sings_measure() {
    DynamicJsonDocument vitals_doc(32);
    unsigned long start_time = millis(), sample_time = millis();
    float mean_HEART_RATE = 0.0f, mean_BREATH_RATE = 0.0f;
    int heart_rate_points = 0, breath_rate_points = 0, data_size = 0;
    while (millis() - start_time < (unsigned long) MEASURE_TIME*mS_S) {
        radar.Breath_Heart();
        switch(radar.sensor_report) {
            case HEARTRATEVAL:
                heart_rate_points++;
                mean_HEART_RATE = (mean_HEART_RATE*(heart_rate_points-1)+radar.heart_rate)/heart_rate_points;
                break;
            case BREATHVAL:
                breath_rate_points++;
                mean_BREATH_RATE = (mean_BREATH_RATE*(breath_rate_points-1)+radar.breath_rate)/breath_rate_points;
                break;
            default:
                break;  
        } 
        if (millis() - sample_time > (unsigned long) SAMPLE_TIME*mS_S) {
            JsonObject sample_array = vitals_doc.createNestedObject("sample "+String(data_size)); 
            sample_array["timestamp"] = millis();
            sample_array["heartrate"] = mean_HEART_RATE;
            sample_array["breathrate"] = mean_BREATH_RATE;
            data_size++;
            mean_HEART_RATE = 0.0f;
            mean_BREATH_RATE = 0.0f;
            heart_rate_points = 0;
            breath_rate_points = 0;
            sample_time = millis();
        }
    }
    radar.reset_func();
    vitals_doc["data_size"] = data_size;
    String serialized_vitals;
    serializeJson(vitals_doc, serialized_vitals);
    return serialized_vitals.c_str();
}