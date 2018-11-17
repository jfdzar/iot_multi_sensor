/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

 *************************************************************
 IoT Multi Sensor Project

 Values from different sensors are read and sent over the Blynk API
 Alerts are also generated depending on the Sensor Values
 Following Sensors are implemented at the moment:
 * Temperature from BME280
 * Humiditz from BME280
 * Air Pressure from BME280
 * Water Leak Detector - 1
 *************************************************************/
/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial

#define WATER_SENSOR_PIN 16
#define BUZZER_PIN 15
#define TIMER_INTERVAL 30000L //Time in ms
#define ALARM_LENGTH 1000 //in ms Delay will be 10x 

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "cactus_io_BME280_I2C.h"
#include "WiFiSettings.h"

//Global Variables for BME280 Measurements
BME280_I2C bme(0x76); // I2C
float h, t, p;
bool water_alarm = false;

//Read the Tokens and WiFi Credentials from constants define in pass.h
const char auth[] = TOKEN;
const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASS;

BlynkTimer timer_sensors;

WidgetLED led(V0);

void getWeather() {
    bme.readSensor();
    h = bme.getHumidity();
    t = bme.getTemperature_C();
    p = bme.getPressure_MB();    
    delay(100);
}

bool detectWater(){
  int SensorState = 0;
  SensorState = digitalRead(WATER_SENSOR_PIN);
  if (SensorState == 1){
    return true;
  }
  else{
    return false;
  }
}

void sendAlarmMail(){
  if(!water_alarm){
      Blynk.email("Water Alarm", "Hola Juani, hemos detectado agua en tu baño");
      water_alarm = true;
   }
}

void readAndSendSensors(){
  // In the app, Widget's reading frequency should be set to PUSH. This means
  // that you define how often to send data to Blynk App.
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  //Serial.println("Reading Sensors");
  getWeather();
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V4, h);
  Blynk.virtualWrite(V3, p);

  if (detectWater()){
    led.off();
  }
  else{
    led.on();
    sendAlarmMail();
  }
}

void initBMESensor(){
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void setup(){
  // Debug console
  Serial.begin(9600);
  //Initialize Temperatur, Humidity, Pressure Sensor
  initBMESensor();
  //Connect to the Wifi Network
  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called TIMER_INTERVAL 
  timer_sensors.setInterval(TIMER_INTERVAL, readAndSendSensors);
  //Define Buzzer Pin as an Output
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop(){
  Blynk.run();
  timer_sensors.run();

  if (water_alarm){
    //Serial.println("ALARM!");
    tone(BUZZER_PIN,500);
    delay(ALARM_LENGTH);
  }
  noTone(BUZZER_PIN);
  delay(ALARM_LENGTH*10);
}
