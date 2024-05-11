/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#ifndef _WIFI_MQTT_UTILS_H__
#define _WIFI_MQTT_UTILS_H__

#include <WiFi.h>
#include <PubSubClient.h>

#define network_attempt_time 100000 // in miliseconds

// WiFi
const char *ssid = "clarc1";
const char *password = "robotclarc1";

// MQTT
const char* mqtt_server = "192.168.0.164";
const int mqtt_port = 1122;
const char* mqtt_client_name = "cmaESP";

void WiFi_config();
void MQTT_config();

#endif