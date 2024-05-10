/*
@author Carlos Madrid Espinosa
@date 2024/04/19
*/

#include "Arduino.h"
#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"

#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "clark1";
const char *password = "robotclarc1";

const char* mqtt_server = "192.168.0.164";
const int mqtt_port = 1122;
const char* MQTT_CLIENT_NAME = "cmaESP";

WiFiClient espClient; 
PubSubClient client(espClient);

void setup() {
  // Conexión con el monitor serial
  Serial.begin(115200);
  delay(1000);
  while (!Serial);
  Serial.println("Canal de Comunicación establecido");
  // Conexión con el sensor
  Sensor_Serial.begin(115200, SERIAL_8N1, RX, TX);
  delay(1000);
  while (!Sensor_Serial);
  Serial.println("Sensor Conectado");

  // Configuración del WiFi: 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Concetándonos a la red WiFi");
  }
  Serial.println("WiFi Conectado");


  // Configuración de MQTT
  /*
  Serial.println("Conectando con el Broker MQTT");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(OnMqttReceived);
  while(!client.connected()) {
    delay(500);
    conecta_MQTT();
  }
  
  client.publish("/cmaESP32/test", "Hola, soy el ESP");
  */

  // Aumentamos en 1 el número de inicios del sistema
  bootCount++;
  Serial.println("==========================================================");
  Serial.println("Boot " + String(bootCount));
  Serial.println("Setup finalizado exitosamente.");
  Serial.println("----------------------------------------------------------");
  // Comenzamos buscando personas en rango operativo del sensor
  Serial.println("Escaneando el perímetro.");
  if (person_detec()) {
    // Detectamos a alguien y calculamos su posición
    Serial.println("Persona detectada.");
    Serial.print("A ");
    Serial.print(distance);
    Serial.println(" m.");
    Serial.print("En posición relativa:");
    Serial.print("X: ");
    Serial.print(direction[0]);
    Serial.print(" m, Y: ");
    Serial.print(direction[1]);
    Serial.print(" m, Z: ");
    Serial.print(direction[2]);
    Serial.println(" m.");
    Serial.println("----------------------------------------------------------");
    // Si se detectan personas se avisa al servidor y esperamos su respuesta
    Serial.println("Notificando al Broker");
    if (aviso_persona_detectada()) {
      Serial.println("Confirmación recibida");
      Serial.println("----------------------------------------------------------");
      // Si recibimos confirmación por parte del servidor comenzamos a medir sus variables fisiológicas
      Serial.println("Midiendo variables:");
      vital_sings_measure();
      Serial.println("----------------------------------------------------------");
    } else {
      // Si el servidor nos lo conforma pasamos al estado base
      Serial.println("Confirmación no recibida o denadgada");
      Serial.println("----------------------------------------------------------");
    }
  } else {
    // Si no se ha detectado una persona pasamos al estado base
    Serial.println("No se ha dectectado a nadie en el rango efectivo");
    Serial.println("----------------------------------------------------------");
  }
  // Si no podemos hacer nada o ya hemos terminado dormimos al dispositivo
  delay(1000);
  esp_sleep_enable_timer_wakeup(SLEEP_TIME*uS_S);
  Serial.println("Inicio del sueño profundo");
  Serial.println("El sistema se dormirá " + String(SLEEP_TIME) + " segundos");
  Serial.println("----------------------------------------------------------");

  // Expulsamos todo de los canales seriales y empezamos el sueño profundo
  Serial.flush();
  Sensor_Serial.flush();
  esp_deep_sleep_start();
}

/*
String content = "";
void OnMqttReceived(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Received on ");
  Serial.print(topic);
  Serial.print(": ");

  content = "";  
  for (size_t i = 0; i < length; i++) {
    content.concat((char)payload[i]);
  }
  Serial.print(content);
  Serial.println();
}
*/


// Conecta el cliente con el servidor MQTT
/*
void conecta_MQTT() {
  Serial.println("Intentando reconectar con el servidor MQTT");
  String client_id = "esp8266-client-";
  client_id += String(WiFi.macAddress());
  if (client.connect(client_id.c_str())) {
    Serial.println("Conextados al servidor MQTT");
  }
  else {
    Serial.print("Conexión fallida, Código: ");
    Serial.println(client.state());
  }
}
*/

long ultimoMsg = 0;
// Avisa de que se ha detectado a una persona y le envía un 
// JSON con sus datos de posición y distancia. Después espera a 
// que se confirme que puede seguir
bool aviso_persona_detectada() {
  /*StaticJsonDocument<80> doc;
  char output[80];
  long tiempo = millis();
  doc["d"] = distancia;
  doc["x"] = direccion[0];
  doc["y"] = direccion[1];
  doc["z"] = direccion[2];
  serializeJson(doc, output);
  client.publish("/cmaESP32/test", "Hola, soy el ESP");
  */
  return true;
}

void loop() {
  // Por el sleep no usamos el loop
}