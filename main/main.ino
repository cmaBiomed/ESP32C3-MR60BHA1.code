

/*
@Autor Carlos Madrid Espinosa
*/

#include "Arduino.h"
#include "60ghzbreathheart.h"
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <HardwareSerial.h>
#include <esp_sleep.h>
#include <WiFi.h>
#include <PubSubClient.h>


// Para pasar de us y ms a s
#define FE_uS_S 1000000
#define FE_mS_S 1000
// Número de unidades de tiempo que va a dormirse
#define T_dormido 5
// Segundos en los que medimos y detectamos personas respectivamente
#define T_medida 20
#define T_deteccion 20

// Pines UART para el sensor
#define Rx 18
#define Tx 19

// Comunicación serial con el sensor
HardwareSerial Sensor_Serial(1);
// le damos acceso al sensor a este canal
BreathHeart_60GHz radar = BreathHeart_60GHz(&Sensor_Serial);

// Wifi
const char *ssid = "clarc1"; // Nombre de la red
const char *password = "robotclarc1";  // Contraseña de la red

// Mqtt
const char* mqtt_serv = ""; // ip del servidor
const int mqtt_port = 1883; // puerto del servidor

WiFiClient espClient; 
PubSubClient client(espClient);

// número de veces que hemos iniciado el sistema
RTC_DATA_ATTR int bootCount = 0;

// Orientación de la persona detectada relativa al sensor
float* direccion = new float[3];
// Distancia a la persona detectada
float distancia; 

void setup() {
  // Conexión con el monitor serial
  Serial.begin(115200);
  delay(1000);
  while (!Serial);
  Serial.println("Canal de Comunicación establecido");
  // Conexión con el sensor
  Sensor_Serial.begin(115200, SERIAL_8N1, Rx, Tx);
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

/*
  // Configuración de MQTT
  Serial.println("Conectando con el Broker MQTT");
  client.setServer(mqtt_server, mqtt_port);
  while(!client.connected()) {
    conecta_mqtt();
  }
*/

  // Aumentamos en 1 el número de inicios del sistema
  bootCount++;
  Serial.println("==========================================================");
  Serial.println("Boot " + String(bootCount));
  Serial.println("Setup finalizado exitosamente.");
  Serial.println("----------------------------------------------------------");
  // Comenzamos buscando personas en rango operativo del sensor
  Serial.println("Escaneando el perímetro.");
  if (detecta_personas()) {
    // Detectamos a alguien y calculamos su posición
    Serial.println("Persona detectada.");
    Serial.print("A ");
    Serial.print(distancia);
    Serial.println(" m.");
    Serial.print("En posición relativa:");
    Serial.print("X: ");
    Serial.print(direccion[0]);
    Serial.print(" m, Y: ");
    Serial.print(direccion[0]);
    Serial.print(" m, Z: ");
    Serial.print(direccion[0]);
    Serial.println(" m.");
    Serial.println("----------------------------------------------------------");
    // Si se detectan personas se avisa al servidor y esperamos su respuesta
    Serial.println("Notificando al Broker");
    if (aviso_persona_detectada()) {
      Serial.println("Confirmación recibida");
      Serial.println("----------------------------------------------------------");
      // Si recibimos confirmación por parte del servidor comenzamos a medir sus variables fisiológicas
      Serial.println("Midiendo variables:");
      mide_pulso_respiracion();
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
  esp_sleep_enable_timer_wakeup(FE_uS_S * T_dormido);
  Serial.println("Inicio del sueño profundo");
  Serial.println("El sistema se dormirá " + String(T_dormido) + " segundos");
  Serial.println("----------------------------------------------------------");

  // Expulsamos todo de los canales seriales y empezamos el sueño profundo
  Serial.flush();
  Sensor_Serial.flush();
  esp_deep_sleep_start();
}

// Conecta el cliente con el servidor MQTT
void conecta_MQTT() {
  Serial.println("Intentando reconectar con el servidor MQTT");
  String clientId =" ESP32Client-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str())) {
    Serial.println("Conextados al servidor MQTT");
  }
  else {
    Serial.print("Conexión fallida, Código: ");
    Serial.println(client.state());
  }
}

// Función que nos permite detectar presencia humana 
bool detecta_personas() {
  // tiempo de ejecución actual en milisegundos
  unsigned long T_Inicio = millis();
  // Consideramos que hemos detectado a una persona si tenemos su distancia y orientación
  bool distancia_medida = false;
  bool direccion_medida = false;
  // Contamos el tiempo y si llegamos al límite o detectamos a alguien terminamos
  while (millis() - T_Inicio < T_deteccion*FE_mS_S && !(distancia_medida && direccion_medida)) {
    // Activamos el modo de detección de personas
    radar.HumanExis_Func();
    // Serial.println(radar.sensor_report);
    // Medimos orientaciom
    if(radar.sensor_report == DIREVAL) {
      direccion[0] = radar.Dir_x;
      direccion[2] = radar.Dir_y;
      direccion[3] = radar.Dir_z;
      direccion_medida = true;
    }
    // Medimos distancia
    else if (radar.sensor_report == DISVAL) {
      distancia = radar.distance;
      distancia_medida = true;
    }
  }
  return distancia_medida && direccion_medida;
}


long ultimoMsg = 0;
// Avisa de que se ha detectado a una persona y le envía un 
// JSON con sus datos de posición y distancia. Después espera a 
// que se confirme que puede seguir
bool aviso_persona_detectada() {
  /*
  StaticJsonDocument<80> doc;
  char output[80];
  long tiempo = millis();
  doc["d"] = distancia;
  doc["x"] = posicion[0];
  doc["y"] = posicion[1];
  doc["z"] = posicion[2];
  serializeJson(doc, output);
  client.publish("")
  */
  return true;
}

/*
* Medimos las variables de interés en un intervalo
* Las variables se envían al robot/servidor de forma asíncrona
*/
void mide_pulso_respiracion() {
  
  // Tiempo en el que empezamos a medir;
  unsigned long T_inicio = millis();
  unsigned long T_muestra = millis();

  volatile float sum_HEART_RATE = 0;
  volatile float sum_BREATH_RATE = 0;
  volatile int HR_points = 0;
  volatile int BR_points = 0;
  
  // Si seguimos en el tiempo en el que podemos medir
  while (millis() - T_inicio < T_medida*FE_uS_S) {
  
    // Cada muestra dura como máximo 3s:
    if (millis() - T_muestra < 3*FE_uS_S ) {
      radar.Breath_Heart();
      if(radar.sensor_report != 0x00){
        // Si se da un caso de fuera de los rangos lo ignoro, 
        // aunque se podría mandar un aviso o algo
        switch(radar.sensor_report){
            case HEARTRATEVAL:
              sum_HEART_RATE += radar.heart_rate;
              HR_points++;
              break;
            case BREATHVAL:
              sum_BREATH_RATE += radar.breath_rate;
              BR_points++;
              break;
        }
      }
    } 
    else {
      // Si se diese el caso de no medir alguna de las dos, evitamos dividir entre 0
      HR_points = (HR_points == 0) ? 1 : HR_points;
      BR_points = (BR_points == 0) ? 1 : BR_points;

      // mandamos las medidas por el canal serial
      Serial.print("t: ");
      Serial.println(T_inicio-T_muestra);
      Serial.print("HR: ");
      Serial.println(sum_HEART_RATE/(float)HR_points);
      Serial.print("BR: ");
      Serial.println(sum_BREATH_RATE/(float)BR_points);
      // reseteamos todo:
      sum_HEART_RATE = 0;
      sum_BREATH_RATE = 0;
      HR_points = 0;
      BR_points = 0;
      T_muestra = millis(); 
    }
    delay(200); // evitamos atascos
  }
}

void loop() {
  // Por el sleep no usamos el loop
}