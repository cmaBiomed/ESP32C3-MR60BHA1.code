/*
@author Carlos Madrid Espinosa
@date 2024/05/10
*/

#ifndef _RADAR_UTILS_H__
#define _RADAR_UTILS_H__

// UART conection with the sensor
#define Rx 18 // Pin conected to the sensors Tx pin
#define Tx 19 // Pin conected to the sensors Rx pin

// Times for different things (in seconds)
#define SLEEP_TIME      5  // Seconds that the system will sleep
#define MEASURE_TIME    20 // Maximun time (in seconds) for masuring heart rate and breath rate
#define DETECTION_TIME  20 // Maximun time (in seconds) for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

// Positional values from the detected person
extern volatile float person_direction [3];
extern volatile float person_distance;

void sensor_init();
bool person_detec();
void vital_sings_measure();

#endif