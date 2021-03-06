
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Hash.h>
#else
#include <ESP31BWiFi.h>
#endif
#include "ESPAsyncTCP.h"
#include "SyncClient.h"
#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include <Wire.h>
#include <NewPing.h>


const char* ssid = "MOVISTAR_D888";
const char* password = "z33PJKNCNUH7JRPUNXNN";
const int triggerPin = 16;
const int echoPin = 14;
const int MAX_DISTANCE = 200;

NewPing sonar(triggerPin, echoPin, MAX_DISTANCE);
AsyncWebServer server(80);
Adafruit_MPU6050 mpu;


void setup() {
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initMPU();

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });


  server.on("/temperatura", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatura().c_str());
  });

  server.on("/proximidad", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", distancia().c_str());
  });
  
  // Start server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}

String temperatura() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //Lectura de la temperatura en un instante
  float t = temp.temperature;
  return String(t);
}


String distancia() {
  float d = sonar.ping_cm();
  return String(d);
}

void initMPU(){
  if (!mpu.begin()) {
    Serial.println("Chip MPU no Encontrado");
    while (1) {
      yield();
    }
  }
  Serial.println("MPU Encontrado!");
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("A ocurrido un Error montando las SPIFFS");
  }
  Serial.println("SPIFFS Montadas Satisfactoriamente");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Conecando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
}
