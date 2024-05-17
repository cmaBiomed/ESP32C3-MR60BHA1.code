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
const char *ssid = "Cma";
const char *password = "bosquimano";
*/
const char *ssid = "clarc1";
const char *password = "robotclarc1";

// MQTT
const char* mqtt_server  = "192.168.0.164";
const int mqtt_port = 1122;
const char* mqtt_client_name = "cmaESP";
const char* mqtt_client_passw = "pasword";  

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

// Initialization of the MQTT client
WiFiClient espClient; 
PubSubClient client(espClient);

/*
* Configuration for the MQTT client.
*/
void MQTT_config() {
    
}

#endif /*_WIFI_MQTT_UTILS__*/