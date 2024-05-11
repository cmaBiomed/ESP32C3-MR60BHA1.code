/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#include "WiFi_MQTT_utils.h"

/*
* @brief WiFi configuration. It tries the defined ssid and password until 
* we conect to the network. After some defined time we can consider that 
* the concetion is failing for some reason, so we can stop. It may take 
* longer first time the system conects to a network. 
*/
void WiFi_config() {
  WiFi.begin(ssid, password);
  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start_time < network_attempt_time) {
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