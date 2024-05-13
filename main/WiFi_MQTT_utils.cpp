/*
@author Carlos Madrid Espinosa
@date 2024/05/11
@brief This file includes all the wifi and mqtt utilities developed. They consinst on
* configuration for both services and ways of reciving, sending and parsing information
* from an server based MQTT broker.
*/

#ifndef _WIFI_MQTT_UTILS__
#define _WIFI_MQTT_UTILS__

#include "WiFi_MQTT_utils.h"
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
/*
const char *ssid = "Cma";                  // Your networks SSID
const char *password = "bosquimano";       // The password to the network
*/
const char *ssid = "clarc1";                // Your networks SSID
const char *password = "robotclarc1";       // The password to the network


// MQTT
const char* mqtt_server  = "192.168.0.164"; // IP address (or url) of the MQTT broker
const int mqtt_port = 1122;                 // Listener port of the MQTT broker
const char* mqtt_client_name = "cmaESP";    // Username of this device

/*
* WiFi configuration. It tries the defined ssid and password until 
* we conect to the network. After some defined time we can consider that 
* the concetion is failing for some reason, so we can stop. It may take 
* longer first time the system conects to a network. 
*/
void WiFi_config() {
  WiFi.begin(ssid, password);
  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start_time < (unsigned long)network_attempt_time) {
    delay(500);
  }
}

// initialization of the MQTT client
WiFiClient espClient; 
PubSubClient client(espClient);

/*
* Configuration for the MQTT client.
*/
void MQTT_config() {
    
}

#endif /*_WIFI_MQTT_UTILS__*/