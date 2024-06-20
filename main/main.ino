/**
 * @author Carlos Madrid Espinosa
 * @date 2024/04/19
*/

#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"
#include <esp_sleep.h>

#define SLEEP_TIME  15
#define WAIT_TIME   5

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
    if (!person_identified){
      go_to_sleep();
    }
    const char * vitals_serialized_JSON = vital_sings_measure(); 
    delay(200);
    go_to_sleep();
  }
}

void go_to_sleep() {
  esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_TIME*uS_S);
  esp_deep_sleep_start();
}

void loop() {
  const char * detected_serialized_JSON = person_detect();
  delay(200);
}