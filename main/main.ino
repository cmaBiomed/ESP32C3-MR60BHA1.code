/**
 * @author Carlos Madrid Espinosa
 * @date 2024/04/19
*/

#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"
#include <esp_sleep.h>

#define SLEEP_TIME  10
#define WAIT_TIME   20

// UART conection with the UART-USB bridge
#define OUT_RX 6 // RX pin for serial comunication
#define OUT_TX 7 // TX pin for serial cominication

// Serial port for external comunication (OUT_XX pins)
HardwareSerial Output_serial(0);

// RTC_DATA_ATTR int bootCount = 0;

bool distance_mode = false;

void setup() {
  sensor_init();
  if (!WiFi_config()) {
    go_to_sleep();
  }
  if (!MQTT_config()) {
    go_to_sleep();
  }
  if (!distance_mode) {
    const char * detected_serialized_JSON = person_detect();
    if (!person_detected) {
      go_to_sleep();
    }
    MQTT_publish_JSON("cma/person/positional", detected_serialized_JSON);
    unsigned int start_wait_time = millis();
    while (millis()-start_wait_time < (unsigned int) WAIT_TIME*mS_S && !person_identified);
    if (!person_identified){
      go_to_sleep();
    }
    const char * vitals_serialized_JSON = vital_sings_measure(); 
    MQTT_publish_JSON("cma/person/vitals", vitals_serialized_JSON);
    go_to_sleep();
  }
}

void go_to_sleep() {
  esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_TIME*uS_S);
  esp_deep_sleep_start();
}

void loop() {
  const char * detected_serialized_JSON = person_detect();
  MQTT_publish_JSON("cma/person/positional", detected_serialized_JSON);
  delay(500);
}