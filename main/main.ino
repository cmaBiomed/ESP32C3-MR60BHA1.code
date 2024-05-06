/*
@Autor Carlos Madrid Espinosa
*/

#include "Arduino.h"
#include "60ghzbreathheart.h"
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
#define T_medida 10
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
    Serial.println("Concetándonos a la red");
  }
  Serial.println("WiFi Conectado");

  // Configuración de MQTT
  

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
      Serial.print("Puslo Cardíaco: ");
      Serial.println(" ");
      Serial.print("Frecuencia respiratoria: ");
      Serial.println(" ");
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

// Conexíón sincrona con el robot/servidor
bool aviso_persona_detectada() {
  delay(2000);
  return true;
  /*
   TODO
  */
}
/*
* Medimos las variables de interés en un intervalo
* Las variables se envían al robot/servidor de forma asíncrona
*/
void mide_pulso_respiracion() {
  delay(2000);
}

void loop() {
  // Por el sleep no usamos el loop
}