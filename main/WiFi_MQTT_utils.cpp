/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#include "WiFi_MQTT_utils.h"
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "clarc1";                // Your networks SSID
const char *password = "robotclarc1";       // The password to the network

// MQTT
const char* mqtt_server  = "192.168.0.164"; // IP address of the MQTT broker
const int mqtt_port = 1122;                 // Listener port of the MQTT broker
const char* mqtt_client_name = "cmaESP";    // Username of this device

/*
* @brief WiFi configuration. It tries the defined ssid and password until 
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
* @brief Configuration for the MQTT client.
*/
void MQTT_config() {
    
}