#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define servoPin 2   //pin number

#define servoMinAngle 30 //min val (0)
#define servoMaxAngle 40 //max val (180)
#define minTemp 20 //min val (0)
#define maxTemp 40 //max val (180)

Servo servo;

const int oneWireBus = 5;  //seosor on D1
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
  servo.attach(4, 500, 2400);
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Serial.println(String(tempC) +"ºC");
  servo.write(180 - map(tempC*10, minTemp*10, maxTemp*10, 0, 180));
  delay(50);
}
