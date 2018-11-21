#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

// select which pin will trigger the configuration portal when set to LOW
// ESP-01 users please note: the only pins available (0 and 2), are shared 
// with the bootloader, so always set them HIGH at power-up
#define TRIGGER_PIN 0


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);
}


void loop() {
  // is configuration portal requested?
  if ( digitalRead(TRIGGER_PIN) == LOW ) {

  }


  // put your main code here, to run repeatedly:

}
