/*
@author Carlos Madrid Espinosa
@date 2024/05/10
*/

#include "radar_utils.h"

/*
* @brief Function that determines wether a person is located within the vacinity of the sensor.
* It uses the distance and direcction functionalities of the sensor, where we determine that if the sensor
* reports both  of those values during the stablished time, then we can say that a person has been detected. 
* This is an stimation and can fail in some scenarios. 
* Also, the direction measure is not very reliable
*/

bool person_detec() {
    unsigned long Start_Time = millis();
    bool measured_distance = false;
    bool measured_direction = false;
    while (millis() - Start_Time < DETECTION_TIME*mS_S  && !(measured_distance && measured_direction)) { 
        radar.HumanExis_Func();
        if (radar.sensor_report != 0x00) {
            switch (radar.sensor_report) {
                case DISVAL:
                    distance = radar.distance;
                    measured_distance = true;
                    break;
                case DIREVAL:
                    direcction[0] = radar.Dir_x;
                    direcction[1] = radar.Dir_y;
                    direcction[2] = radar.Dir_z;
                    measured_direction = true;
                    break;
                default:
                    break;
            }
        }
        delay(200);
    }
  return measured_distance && measured_direction;
}

/*
* Medimos las variables de interés en un intervalo
* Las variables se envían al robot/servidor de forma asíncrona
*/
void mide_pulso_respiracion() {
    unsigned long start_time = millis();
    unsigned long sample_time = millis();
    volatile float sum_HEART_RATE = 0;
    volatile float sum_BREATH_RATE = 0;
    volatile int heart_rate_points = 0;
    volatile int breath_rate_points = 0;
    while (millis() - start_time < MEASURE_TIME*mS_S ) {
        if (millis() - sample_time < 3*mS_S) {
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
            heart_rate_points = (heart_rate_points == 0) ? 1 : breath_rate_points;
            breath_rate_points = (breath_rate_points == 0) ? 1 : heart_rate_points;
            // This is temporal, while i figure out how to store the valueas untill i send them
            Serial.print("t: ");
            Serial.println((start_time-sample_time)/mS_S);
            Serial.print("HR: ");
            Serial.println(sum_HEART_RATE/(float)heart_rate_points);
            Serial.print("BR: ");
            Serial.println(sum_BREATH_RATE/(float)breath_rate_points);
            // rest all values
            sum_HEART_RATE = 0;
            sum_BREATH_RATE = 0;
            heart_rate_points = 0;
            breath_rate_points = 0;
            sample_time = millis(); 
        }
    }
    delay(200);
}

