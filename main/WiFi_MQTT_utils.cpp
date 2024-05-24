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
const char *ssid = "clarc1";
const char *password = "robotclarc1";

// MQTT
IPAddress mqtt_server(192,168,0,117); // Could also use the servers URL
const int mqtt_port = 1883;

// Client credentials
const char* mqtt_client_name = "cmaESP";
const char* mqtt_client_passw = "pasword"; // if on allow anonymous mode, the pasword is optional

// Initialization of the MQTT client
WiFiClient espClient; 
PubSubClient client(espClient);

// Check wether the detected person is identified by the broker
bool person_identified = false;

/*
* WiFi configuration. It tries the defined ssid and password until 
* we conect to the network. After some defined time we can consider that 
* the conetion has failed for some reason, so it stops tryng. It may take 
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

/*
* Configuration for the MQTT client. It sets the url/ip and listener port of the server,
* the it tries conencting to it using the specified client username and a random identifier. 
* If it conects succesfully it also sets the callback, and subscribes to the necesary topics
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
  // client.subscribe("person/identified");
  return client.connected();
}

/*
* It wais for the sercer to confirm that the vital sings meashuring phase has to start
* It calls for the client loop, untill a callback sets the bool:person_identified true, 
* or it runs out of time. 
@note I think i can add this to the main.ino itsefl to avoid makin this overcrowded
*/
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
@param char*:topic          The topic that the client is subscribed to, that recibed the callback
@param byte*:payload        The information asociated to that topic
@param usigned_int:length   Length of the payload  
*/
void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  if (topic == "person/identified") {
    String data = "";
    for(uint i = 0; i < length; i++)
    {
      data += (char)payload[i];
    }
    person_identified = (data == "Y");
  }
}

/*
* We take the recorded distance as input and publish it to the topic "person/distance". 
* We create a static json document that holds the positional values recorded with their time stamp
@param positional_values:values  The struct that we use to store the recorded positional values obtained 
*/
String MQTT_publish_distance(positional_values values) {
  StaticJsonDocument<32> doc;
  doc["time_stamp"] = values.time_stamp;
  doc["distance"] = values.distance;
  char output[55];
  serializeJson(doc, output);
  client.publish("person/distance", output);
  return output;
}

/*
* It creates a dynamic Json file that stores as sub files all the recorded vital sings samples. 
* Each subfile contains 3 fields, the hart and breath rates, and the sample's time stamp
@param recorded_vital_sings*:vitals  Pointer to the array of vital sings that we have recorded from 
* the detected person 
*/
String MQTT_publish_vitals(recorded_vital_sings* vitals) {
  StaticJsonDocument<1024> doc;
  for (size_t i = 0; i < data_size; i++) {
    String sample_id = "Sample "+String(i);
    JsonArray sample_array = doc.createNestedArray(sample_id);
    sample_array.add(vitals[i].sample_time);
    sample_array.add(vitals[i].mean_sample_heart_rate);
    sample_array.add(vitals[i].mean_sample_breath_rate);
  }
  String output;
  serializeJson(doc, output);
  client.publish("person/vitals", output.c_str());
  return output;
}

#endif /*_WIFI_MQTT_UTILS__*/