/*
@author Carlos Madrid Espinosa
@date 2024/05/10
*/

#include "60ghzbreathheart.h"
#include <HardwareSerial.h>
#include <esp_sleep.h>

// UART conection with the sensor
#define RX 18 
#define TX 19

HardwareSerial Sensor_Serial(1);
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

// Times for different things (in seconds)
#define SLEEP_TIME      5  // Time that the system will sleep
#define MEASURE_TIME    20 // Maximun time for masuring heart rate and breath rate
#define DETECTION_TIME  20 // Maximun time for searching people in the person detection mode

// Scale factors
#define uS_S 1000000 // Micro seconds to seconds
#define mS_S 1000    // Mili seconds to seconds

RTC_DATA_ATTR int bootCount = 0;

float direction[3];
float distance;

bool person_detec();
void vital_sings_measure();
