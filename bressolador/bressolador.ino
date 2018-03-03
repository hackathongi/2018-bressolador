/*
  Hackathon Girona 2018 Starter Kit example
  Crib balancer using servos conectec through Orion ContextBroker
  Created by Jordi Corominas
  http://www.hackathongi.cat
*/

// include the Servo library
#include <Servo.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "hackathongi"           // cannot be longer than 32 characters!
#define WLAN_PASS       "tarla2018"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.


Servo myServo;  // create a servo object

void setup() {

  Serial.begin(115200); // open a serial connection to your computer

  Serial.println(F("Hello, CC3000!\n")); 

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  // Optional SSID scan
  // listSSIDResults();
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));

}

void moveCrib()
{
  myServo.attach(9); // attaches the servo on pin 9 to the servo object

  for (int i = 0; i < 1; i++)
  {
    myServo.write(0);
    Serial.println("angle: 0");
    delay(10000);
    myServo.write(180);
    Serial.println("angle: 180");
    delay(10000);
  }
  myServo.detach();
}

void loop() {
  // set the servo in motion
  // moveCrib();
  delay(1000);
}
