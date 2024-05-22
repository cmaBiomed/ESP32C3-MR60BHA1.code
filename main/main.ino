/*
@author Carlos Madrid Espinosa
@date 2024/04/19
*/

// #include "Arduino.h"
#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"
#include <esp_sleep.h>

// UART conection with the UART-USB bridge
#define OUT_RX 6 // RX pin for serial comunication
#define OUT_TX 7 // TX pin for serial cominication

// Serial port for external comunication (OUT_XX pins)
HardwareSerial Output_serial(0);

RTC_DATA_ATTR int bootCount = 0;

void setup() {
  // Output_serial.begin(115200, SERIAL_8N1, OUT_RX, OUT_TX);
  // while(!Output_serial);
  // Output_serial.println("Comunication ouput channel stablished.");
  sensor_init();
  // Output_serial.println("Sensor initialized.");
  if(WiFi_config()) {
    // Output_serial.println("Conected to wifi.");
    if (MQTT_config()) {
      // Output_serial.println("MQTT server found and subscribed to the especified topics.");
      // Output_serial.println("Searching for people.");
      positional_values pos_values = person_detect();
      // Output_serial.print("Person detected at: ");
      // Output_serial.print(person_distance);
      // Output_serial.println("m.");
      MQTT_publish_distance(pos_values);
      // Output_serial.println("Distance published");
      if ( 1 == 1 /* MQTT_wait_for_response() */) {
        // Output_serial.println("Person confirmed to be identified."); 
        //Output_serial.println("Reading vital sings.");
        recorded_vital_sings *vitals =  vital_sings_measure();
        if (vitals != nullptr) {
          MQTT_publish_vitals(vitals);
          // Output_serial.println("Vital sings published");
          // print_vitals(vitals);
        }
        else {
          // Output_serial.println("The system run out of memory unexpectedly");
        }
      }
      else {
        // Output_serial.println("No person was detected or detection time run out");
      }
    }
    else {
      // Output_serial.println("Failed conecting to MQTT");
    }
  }
  else {
    // Output_serial.println("Failed conecting to WiFI");
  }
  // Output_serial.println("Starting deep sleep for: " + String(SLEEP_TIME)+"s.");
  esp_sleep_enable_timer_wakeup(SLEEP_TIME*uS_S);
  esp_deep_sleep_start();
}

void print_vitals(recorded_vital_sings *vitals_array) {
    Output_serial.println("Recorded Vitals: ");
    for(int i = 0; i < static_cast<int>(data_size) - 1; i++) {
        Output_serial.print(vitals_array[i].sample_time);
        Output_serial.print(":");
        Output_serial.print(vitals_array[i].mean_sample_heart_rate);
        Output_serial.print(",");
        Output_serial.print(vitals_array[i].mean_sample_breath_rate);
        Output_serial.println(";");
    }
    Output_serial.println("---------------"); 
    free(vitals_array);
}

void loop() {
  // Por el sleep no usamos el loop
}