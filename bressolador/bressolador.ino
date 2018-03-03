/*
  Hackathon Girona 2018 Starter Kit example
  Crib balancer using servos conectec through Orion ContextBroker
  Created by Jordi Corominas
  http://www.hackathongi.cat
*/

// include the Servo library
#include <Servo.h>

Servo myServo;  // create a servo object

void setup() {
  Serial.begin(9600); // open a serial connection to your computer
}

void moveCrib()
{
  myServo.attach(9); // attaches the servo on pin 9 to the servo object

  for (int i = 0; i < 1; i++)
  {
    myServo.write(0);
    Serial.println("angle: 0");
    delay(5000);
    myServo.write(180);
    Serial.println("angle: 180");
    delay(5000);
  }
  myServo.detach();
}

void loop() {
  // set the servo in motion
  moveCrib();
  delay(1000);
}
