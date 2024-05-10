/*
@author Carlos Madrid Espinosa
@date 2024/05/10
*/

#ifndef _RADARUTILS_H__
#define _RADARUTILS_H__

// UART conection with the sensor
#define RX 18 
#define TX 19

// Times for different things (in seconds)
#define SLEEP_TIME      5  // Time that the system will sleep
#define MEASURE_TIME    20 // Maximun time for masuring heart rate and breath rate
#define DETECTION_TIME  20 // Maximun time for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

extern float person_direction [3];
extern float person_distance;

void sensor_init();
bool person_detec();
void vital_sings_measure();

#endif