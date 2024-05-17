/*
@author Carlos Madrid Espinosa
@date 2024/05/11
*/

#ifndef _WIFI_MQTT_UTILS_H__
#define _WIFI_MQTT_UTILS_H__

// WiFi
extern const char *ssid; // Your networks SSID
const char *password;    // The password to the network

// MQTT server addresing
const IPAddress mqtt_server; // IP address (or url) of the MQTT broker
const int mqtt_port;         // Listener port of the MQTT broker

// MQTT Client credentials
const char* mqtt_client_name;  // Username of this device
const char* mqtt_client_passw; // Password of this device

#define network_attempt_time 100000 // in miliseconds

void WiFi_config();
void MQTT_config();

#endif /*_WIFI_MQTT_UTILS_H__*/