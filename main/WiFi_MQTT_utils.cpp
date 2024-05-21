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

// WiFi
/*
const char *ssid = "Cma";
const char *password = "bosquimano";
*/
const char *ssid = "clarc1";
const char *password = "robotclarc1";

// MQTT
IPAddress mqtt_server(192,168,0,117);
const int mqtt_port = 1883;

// Client credentials _Optional_
const char* mqtt_client_name = "cmaESP";
const char* mqtt_client_passw = "pasword";  

/*
* WiFi configuration. It tries the defined ssid and password until 
* we conect to the network. After some defined time we can consider that 
* the concetion is failing for some reason, so we can stop. It may take 
* longer first time the system conects to a network. 
*/
bool WiFi_config() {
  WiFi.begin(ssid, password);
  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start_time < (unsigned long)network_attempt_time) {
    delay(500);
  }
  return WiFi.status() == WL_CONNECTED;
}

// Initialization of the MQTT client
WiFiClient espClient; 
PubSubClient client(espClient);

bool person_identified = false;

/*
* Configuration for the MQTT client.
*/
bool MQTT_config() {
  client.setServer(mqtt_server, mqtt_port);
  unsigned int start_time = millis();
  while (!client.connected() && millis()-start_time < (unsigned int)network_attempt_time) {
    String clientId = "cmaESP32-";
    clientId += String(random(0xffff), HEX);    
    client.connect(clientId.c_str());
    client.loop();
  }
  client.setCallback(MQTT_callback);
  client.subscribe("person/identified");
  return client.connected();
}

bool MQTT_wait_for_response() {
  unsigned int start_time = millis();
  while (millis()-start_time < (unsigned int)network_attempt_time && !person_identified)
  {
    client.loop();
  }
  return person_identified;
}

/*
* Call back for this system, it recives information from the topics that 
* the client is subscribed to. In this case "person/identified". It requires
* the program calling client.loop() to be invoqued
*/
void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  if (topic == (char*)"person/identified") {
    String data = "";
    for(uint i = 0; i < length; i++)
    {
      data += (char)payload[i];
    }
    person_identified = (data == "Y");
  }
}


/*
* We take the recorded distance as input and publish it to the topic "person/distance"
*/
void MQTT_publish_distance(float person_distance) {
  /* Unnecesary af
  constexpr size_t BUFFER_SIZE = 7;
  char buffer[BUFFER_SIZE]; 
  dtostrf(person_distance, BUFFER_SIZE - 1, 2, buffer);
  */
  StaticJsonDocument<32> doc;
  doc["distance"] = person_distance;
  char output[55];
  serializeJson(doc, output);
  client.publish("person/distance", output);
}

void MQTT_publish_vitals(recorded_vital_sings *vitals) {
  DynamicJsonDocument doc(1024); 
  JsonArray vitalsArray = doc.createNestedArray("vitals");
  for (size_t i = 0; i < data_size; i++) {
    JsonObject measure = vitalsArray.createNestedObject();
    measure["time"] = vitals[i].sample_time;
    measure["heart_rate"] = vitals[i].mean_sample_heart_rate;
    measure["breath_rate"] = vitals[i].mean_sample_breath_rate;
  }
  String output;
  serializeJson(doc, output);
  client.publish("person/vitals", output.c_str());
}


#endif /*_WIFI_MQTT_UTILS__*/