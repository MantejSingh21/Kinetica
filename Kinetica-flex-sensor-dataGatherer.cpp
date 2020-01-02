/*
  Using the Qwiic Flex Glvoe Controller
  By: Andy England
  SparkFun Electronics
  Date: July 17, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example shows how to output accelerometer values

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 115200 baud to serial monitor.

Mantej's instruction set 
https://learn.sparkfun.com/tutorials/qwiic-flex-glove-controller-hookup-guide/all

Machine Learning Algorithm:
K Nearest Neighbour Algorithm but modified. Instead of classifying it between 2 groups, it will
return the gesture closest to the unknown point. 
Adapted from --> https://www.geeksforgeeks.org/k-nearest-neighbours/

*/

#include <SparkFun_ADS1015_Arduino_Library.h>
#include <Wire.h>
#include <map>
#include <utility>
#include <iostream>
#include <bits/stdc++.h>
#include <Arduino.h>

ADS1015 fingerSensor;
using namespace std;

void setup() {
  
  Wire.begin();
  Serial.begin(115200);
  
  if (fingerSensor.begin() == false) {
     Serial.println("Device not found. Check wiring.");
     while (1);
  } 
  
  fingerSensor.setGain(ADS1015_CONFIG_PGA_TWOTHIRDS); // Gain of 2/3 to works well with flex glove board voltage swings (default is gain of 2)
}

void loop() {  
  uint16_t data;
  for (int finger = 0; finger < 2; finger++) {
    data = fingerSensor.getAnalogData(finger);
    // Graph Output 
    Serial.print(finger);
    Serial.print(": ");
    Serial.print(data);
    Serial.print(",");
  }
  Serial.println();
}

/*
Gesture Recording:
"Hello"
1.0 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.1 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.2 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.3 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.4 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.5 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.6 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.7 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.8 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.9 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)

"Goodbye"
1.0 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.1 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.2 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.3 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.4 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.5 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.6 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.7 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.8 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
1.9 (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0), (0.0,0.0)
*/