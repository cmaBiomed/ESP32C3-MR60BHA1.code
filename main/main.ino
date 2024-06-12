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

RTC_DATA_ATTR int bootCount = 0;

bool distance_mode = true;

void setup() {
  //////////////////////////////////////////////////////////////////
  /*
  bootCount++;
  Output_serial.begin(115200, SERIAL_8N1, OUT_RX, OUT_TX);
  while(!Output_serial);
  Output_serial.println("Comunication ouput channel stablished");
  Output_serial.println("Boot Nº: "+String(bootCount));
  */
  sensor_init();
  // Output_serial.println("Sensor conected");
  if (!WiFi_config()) {
    // Output_serial.println("Failed conecting to WiFI");
    go_to_sleep();
  }
  // Output_serial.println("WiFi conection stablished");
  if (!MQTT_config()) {
    // Output_serial.println("Failed conecting to MQTT server");
    go_to_sleep();
  }
  // Output_serial.println("MQTT setup compleated");
  //////////////////////////////////////////////////////////////////
  // Output_serial.println("Searching for people.");
  if (!distance_mode) {
    const char * detected_serialized_JSON = person_detect();
    if (!person_detected) {
      // Output_serial.println("No person was detected");
      go_to_sleep();
    }
    /*
    Output_serial.print("Person detected: ");
    Output_serial.println(detected_serialized_JSON);
    */
    MQTT_publish_JSON("cma/person/positional", detected_serialized_JSON);
    /*
    Output_serial.println("Published positional values");
    Output_serial.println("Waiting for response");
    */
    unsigned int start_wait_time = millis();
    while (millis()-start_wait_time < (unsigned int) WAIT_TIME*mS_S && !person_identified);
    if (!person_identified){
      // Output_serial.println("The detected person was not identified");
      go_to_sleep();
    }
    /*
    Output_serial.println("The prerson detected has been identified");
    Output_serial.println("Reading vital sings");
    */
    const char * vitals_serialized_JSON = vital_sings_measure(); 
    /*
    Output_serial.print("Vital sings red: ");
    Output_serial.println(vitals_serialized_JSON);
    */
    MQTT_publish_JSON("cma/person/vitals", vitals_serialized_JSON);
    // Output_serial.println("Published vital sings");
    go_to_sleep();
  }
}

void go_to_sleep() {
  // Output_serial.println("Starting deep sleep for: " + String(SLEEP_TIME)+"s.");
  esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_TIME*uS_S);
  esp_deep_sleep_start();
}

void loop() {
  // If not in distabce mode this is never used
  const char * detected_serialized_JSON = person_detect();
  MQTT_publish_JSON("cma/person/positional", detected_serialized_JSON);
  delay(500);
}