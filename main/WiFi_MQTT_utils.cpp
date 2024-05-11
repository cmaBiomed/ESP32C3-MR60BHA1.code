/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#include "WiFi_MQTT_utils.h"

void WiFi_config() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void MQTT_config() {
    
}