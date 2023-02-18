////////////////////////////////////////////////
#include <Servo.h>   //Servo
#include <OneWire.h> //Communication
#include <DallasTemperature.h> //Thermometer
#include <Arduino_JSON.h>  //json
////////////////////////////////////////////////
#include <ESP8266WiFi.h> //WiFi
//#include <ArduinoOTA.h>  //Authentication
#include <ESP8266WebServer.h> //Web Server
#include "FS.h"               //File System
///////////////////////////////////////////////

#define servoPin 2   //pin number
int minAngle = 0, maxAngle = 0, minTemp = 0, maxTemp = 0;
//int minAngle = 0; //min val (0)
//int maxAngle = 180; //max val (180)
//int minTemp = 20; //min val in degree c
//int maxTemp = 40; //max val in degree c

const char* ssid = "Thermostat";
const char* password = "12345678";
const char* www_username = "admin";
const char* www_password = "1234";

Servo servo;
const int oneWireBus = 5;  //seosor on D1
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
WiFiClient client;
ESP8266WebServer server(80);

String returnFile(String fileName) {
  //  long tim = millis();
  File file = SPIFFS.open(fileName, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "Error";
  }
  String content = "";
  while (file.available()) {
    content = content + file.readString();
  }
  file.close();
  //  Serial.println(millis() - tim);
  return content;
}

void saveFile(String content, String fileName) {
  File file = SPIFFS.open(fileName, "w");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  file.print(content);
  file.close();
}

void createWiFi() {

  IPAddress local_ip(192, 168, 0, 1);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);
}

void beginServer() {
  server.on("/", []() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/html", returnFile("/page.html"));

  });
  server.on("/page.css", []() {
    server.send(200, "text/css", returnFile("/page.css"));

  });
  server.on("/get", []() {

    minTemp = server.arg("min_temp").toInt();
    maxTemp = server.arg("max_temp").toInt();
    minAngle = server.arg("min_angle").toInt();
    maxAngle = server.arg("max_angle").toInt();
    server.send(200, "text/html", returnFile("/done.html"));

    String json = "{\"minTemp\":" + String(minTemp) + ",\"maxTemp\":" + String(maxTemp) + ",\"minAngle\":" + String(minAngle) + ",\"maxAngle\":" + String(maxAngle) + "}";
//    Serial.println(json);
    saveFile(json, "/data.txt");
  });
  server.begin();
}

void initiateVariables() {
  JSONVar root = JSON.parse(returnFile("/data.txt").c_str());

  minTemp = root["minTemp"];
  maxTemp = root["maxTemp"];
  minAngle = root["minAngle"];
  maxAngle = root["maxAngle"];
}

void setup() {
  createWiFi();

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  initiateVariables();
  beginServer();

  servo.attach(4, 500, 2400);
  Serial.begin(115200);
  sensors.begin();
  Serial.println("");
  //  Serial.println(returnFile("/data.txt"));
}

void loop() {
  Serial.println(String(minTemp) + String(maxTemp) + String(minAngle) + String(maxAngle));
  server.handleClient();
//  sensors.requestTemperatures();
//  float tempC = sensors.getTempCByIndex(0);
//  //  Serial.println(String(tempC) + "ºC");
//    servo.write(180 - map(tempC * 10, minTemp * 10, maxTemp * 10, 0, 180));
//  delay(50);
}
