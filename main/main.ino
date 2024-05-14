/*
@author Carlos Madrid Espinosa
@date 2024/04/19
*/

// #include "Arduino.h"
#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"

#include <esp_sleep.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// UART conection with the UART-USB bridge
#define OUT_RX 4 // RX pin for serial comunication
#define OUT_TX 5 // TX pin for serial cominication

RTC_DATA_ATTR int bootCount = 0;

void setup() {

  Output_serial.begin(115200, SERIAL_8N1, OUT_RX, OUT_TX);
  while(!Output_serial);
  Output_serial.println("Comunication ouput channel stablished");
  
  // Set the comunication protocol
  set_Comunication_Mode(0);
  Output_serial.print("Use MQTT: ");
  Output_serial.println(USE_MQTT);

  // we initilize the sensor
  sensor_init();
  Output_serial.println("Sensor initialized");

  if (USE_MQTT) {
    WiFi_config();
    Output_serial.println("WiFI Correctly stablished");
    // @todo print the IP of the device
    // @todo Mqtt things go here
  }
  
  Output_serial.println("Searching for people");
  if (person_detec()) {
    vital_sings_measure();
  }
  esp_sleep_enable_timer_wakeup(SLEEP_TIME*uS_S);
  esp_deep_sleep_start();
}

void loop() {
  // Por el sleep no usamos el loop
}