/*
 * Universidad Privada Antenor Orrego (UPAO)
 * Escuela de Ingeniería de Sistemas / Electrónica
 * Asignatura: Internet de las Cosas (IoT) - Semana 11
 *
 * Proyecto: Monitoreo y Control con ESP32 y Blynk IoT (Hardware Físico)
 */

// 1. Credenciales Oficiales de tu Blynk Console
#define BLYNK_TEMPLATE_ID "TMPL2Pz9GZGrL"
#define BLYNK_TEMPLATE_NAME "Practica Calificada"
#define BLYNK_AUTH_TOKEN "j-6cX1kiKaQS5F6d1A5QLGN9W3-_--bq"

#define BLYNK_PRINT Serial

// 2. Inclusión de librerías
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// 3. Credenciales de la red WiFi Física de tu hogar
char ssid[] = "XIDI";
char pass[] = "18011303";

// 4. Pines físicos correspondientes al circuito
#define LED_ROJO 2      // V0
#define LED_AMARILLO 5  // V1
#define LED_VERDE 18    // V2
#define LED_AZUL 19     // V3
#define DHTPIN 4        // V8, V9, V10 (DATA del sensor DHT11)
#define POT_PIN 34      // V11 (Lectura analógica del potenciómetro)

// 5. Configuración del Sensor DHT11 (Físico)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 6. Instanciación del Temporizador Blynk
BlynkTimer timer;

// ==========================================
// CALLBACKS DE BLYNK (Control y Sincronización)
// ==========================================

// Al conectar a Blynk, sincronizamos el estado de los interruptores para que coincidan con la nube
BLYNK_CONNECTED() {
  Serial.println("¡Conectado a Blynk! Sincronizando pines virtuales...");
  Blynk.syncVirtual(V0, V1, V2, V3);
}

BLYNK_WRITE(V0) { // Control LED ROJO
  int pinValue = param.asInt();
  digitalWrite(LED_ROJO, pinValue);
  Blynk.virtualWrite(V4, pinValue * 255); // Estado LED ROJO (0 a 255 para brillo completo)
  Serial.print("Blynk V0 -> LED ROJO: ");
  Serial.println(pinValue == HIGH ? "ENCENDIDO" : "APAGADO");
}

BLYNK_WRITE(V1) { // Control LED AMARILLO
  int pinValue = param.asInt();
  digitalWrite(LED_AMARILLO, pinValue);
  Blynk.virtualWrite(V5, pinValue * 255); // Estado LED AMARILLO (0 a 255)
  Serial.print("Blynk V1 -> LED AMARILLO: ");
  Serial.println(pinValue == HIGH ? "ENCENDIDO" : "APAGADO");
}

BLYNK_WRITE(V2) { // Control LED VERDE
  int pinValue = param.asInt();
  digitalWrite(LED_VERDE, pinValue);
  Blynk.virtualWrite(V6, pinValue * 255); // Estado LED VERDE (0 a 255)
  Serial.print("Blynk V2 -> LED VERDE: ");
  Serial.println(pinValue == HIGH ? "ENCENDIDO" : "APAGADO");
}

BLYNK_WRITE(V3) { // Control LED AZUL
  int pinValue = param.asInt();
  digitalWrite(LED_AZUL, pinValue);
  Blynk.virtualWrite(V7, pinValue * 255); // Estado LED AZUL (0 a 255)
  Serial.print("Blynk V3 -> LED AZUL: ");
  Serial.println(pinValue == HIGH ? "ENCENDIDO" : "APAGADO");
}

// ==========================================
// LECTURA DE SENSORES Y ALERTAS (2 segundos)
// ==========================================
void sendSensorData() {
  // 1. Lectura del Potenciómetro (V11)
  int adcValue = analogRead(POT_PIN);
  float voltage = (adcValue / 4095.0) * 3.3;
  Blynk.virtualWrite(V11, voltage);

  // 2. Lectura del DHT11 (V8, V9) y Alerta de Error (V10)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("¡Alerta! Fallo al leer el sensor DHT11");
    Blynk.virtualWrite(V10, 1); // Enciende alerta de error (V10 en 1)
  } else {
    Blynk.virtualWrite(V10, 0); // Apaga alerta de error (V10 en 0)
    Blynk.virtualWrite(V8, t);  // Envia Temperatura a V8
    Blynk.virtualWrite(V9, h);  // Envia Humedad a V9
    
    Serial.print("Temp: "); Serial.print(t);
    Serial.print(" °C | Hum: "); Serial.print(h);
    Serial.print(" % | Voltaje: "); Serial.print(voltage);
    Serial.println(" V");
  }
}

// ==========================================
// CONFIGURACIÓN INICIAL (SETUP)
// ==========================================
void setup() {
  Serial.begin(115200);
  
  // Configuración de pines como salidas
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  
  // Inicialización del sensor DHT11
  dht.begin();
  
  Serial.println("Iniciando conexión WiFi...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("¡Dispositivo físico en línea con Blynk Cloud!");

  // Configura temporizador cada 2 segundos
  timer.setInterval(2000L, sendSensorData);
}

// ==========================================
// BUCLE PRINCIPAL (LOOP)
// ==========================================
void loop() {
  Blynk.run();
  timer.run();
}