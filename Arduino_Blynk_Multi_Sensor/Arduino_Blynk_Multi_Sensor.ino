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
  Send the values of a BME280 over the Blynk Interface

 *************************************************************/
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "cactus_io_BME280_I2C.h"
#include "pass.h"

//Global Variables for BME280 Measurements
BME280_I2C bme(0x76); // I2C
float h, t, p;

bool first_time = true;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
const char auth[] = TOKEN;
// Your WiFi credentials.
const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASS;

BlynkTimer timer;

WidgetLED led(V0);

void getWeather() {
    bme.readSensor();
    h = bme.getHumidity();
    t = bme.getTemperature_C();
    p = bme.getPressure_MB();    
    delay(100);
}

bool detectWater(){
  int SensorPin = 14;
  int SensorState = 0;

  SensorState = digitalRead(SensorPin);

  if (SensorState == 1){
    return true;
  }
  else{
    return false;
  }
  
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  getWeather();
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V4, h);
  Blynk.virtualWrite(V3, p);

  if (detectWater()){
    led.off();
    Serial.println("No Water");
  }
  else{
    led.on();
    Serial.println("ALARM");
    if(first_time){
      Blynk.email("Water Alarm", "Hola Juani, hemos detectado agua en tu baño");
      first_time = false;
    }
    
  }

  
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(15000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
