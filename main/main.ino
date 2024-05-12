/*
@author Carlos Madrid Espinosa
@date 2024/04/19
*/

// #include "Arduino.h"
#include "radar_utils.h"
#include "WiFi_MQTT_utils.h"

#include <esp_sleep.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

RTC_DATA_ATTR int bootCount = 0;

void setup() {
  // Conexión con el monitor serial
  Serial.begin(115200);
  delay(1000);
  while (!Serial);
  Serial.println("Canal de Comunicación establecido");
  // Conexión con el sensor
  sensor_init();
  Serial.println("Sensor Conectado");
  // Configuración del WiFi: 
  WiFi_config();
  Serial.println("Wifi Conectado");
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
    Serial.print(person_distance);
    Serial.println(" m.");
    Serial.print("En posición relativa:");
    Serial.print("X: ");
    Serial.print(person_direction[0]);
    Serial.print(" m, Y: ");
    Serial.print(person_direction[1]);
    Serial.print(" m, Z: ");
    Serial.print(person_direction[2]);
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
  esp_deep_sleep_start();
}

bool aviso_persona_detectada() {
  return true;
}

void loop() {
  // Por el sleep no usamos el loop
}