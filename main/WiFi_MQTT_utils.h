/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#ifndef _WIFI_MQTT_UTILS_H__
#define _WIFI_MQTT_UTILS_H__

#include"radar_utils.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define network_attempt_time 10000 // in miliseconds

extern bool person_identified;

bool WiFi_config();
bool MQTT_config();
bool MQTT_wait_for_response();
void MQTT_callback(char* topic, byte* payload, unsigned int length);
String MQTT_publish_distance(positional_values person_distance);
String MQTT_publish_vitals(recorded_vital_sings* vitals);

#endif /*_WIFI_MQTT_UTILS_H__*/