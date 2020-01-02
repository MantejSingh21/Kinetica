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
  // function reads one data points, saves it and then runs the classifier   
  uint16_t data;
  double gesturePoint[data];
  std::pair<double, double> gestureData;
  for (int finger = 0; finger < 2; finger++) {
    data = fingerSensor.getAnalogData(finger);
    // Graph Output 
    Serial.print(finger);
    Serial.print(": ");
    Serial.print(data);
    Serial.print(",");
    // created an array instead of pair because can't do pair.finger
    gesturePoint[finger] = data;
  }
  // saving data into pair to be fed into classifier
  gestureData.first = gesturePoint[0];
  gestureData.second = gesturePoint[1];
  Serial.println(classifierSystem(gestureData));
  
}


struct Point 
{ 
    int val;     // The gesture type 
    double x, y;     // Co-ordinate of finger one, finger 2
    double distance; // Distance from test point 
}; 

// Used to sort an array of points by increasing 
// order of distance 
bool comparison(Point a, Point b) 
{ 
    return (a.distance < b.distance); 
} 

// This function finds classification of point p using 
// k nearest neighbour algorithm. 
int classifyAPoint(Point arr[], int n, int k, Point p) 
{ 
    // Fill distances of all points from p 
    for (int i = 0; i < n; i++) 
        arr[i].distance = 
            sqrt((arr[i].x - p.x) * (arr[i].x - p.x) + 
                 (arr[i].y - p.y) * (arr[i].y - p.y)); 
  
    // Sort the Points by distance from p 
    sort(arr, arr+n, comparison); 

    
    // 10 groups 
    int freq1 = 0;     // Frequency of group 0 
    int freq2 = 0;     // Frequency of group 1 
    int freq3 = 0;     // Frequency of group 2 
    int freq4 = 0;     // Frequency of group 3 
    int freq5 = 0;     // Frequency of group 4 
    int freq6 = 0;     // Frequency of group 5 
    int freq7 = 0;     // Frequency of group 6 
    int freq8 = 0;     // Frequency of group 7 
    int freq9 = 0;     // Frequency of group 8 
    int freq10 = 0;     // Frequency of group9

    for (int i = 0; i < k; i++) 
    { 
        if (arr[i].val == 0) 
            freq1++; 
        else if (arr[i].val == 1) 
            freq2++; 
        else if (arr[i].val == 2) 
            freq3++; 
        else if (arr[i].val == 3) 
            freq4++; 
        else if (arr[i].val == 4) 
            freq5++; 
        else if (arr[i].val == 5) 
            freq6++; 
        else if (arr[i].val == 6) 
            freq7++; 
        else if (arr[i].val == 8) 
            freq8++; 
        else if (arr[i].val == 9) 
            freq9++; 
        else if (arr[i].val == 10) 
            freq10++; 
    } 
    
    //finding the most likely gesture based on frequency in data
    int freqArray[n];
    int max;
    max = freqArray[0];

    freqArray[0] = freq1;
    freqArray[1] = freq2;
    freqArray[2] = freq3;
    freqArray[3] = freq4;
    freqArray[4] = freq5;
    freqArray[5] = freq6;
    freqArray[6] = freq7;
    freqArray[7] = freq8;
    freqArray[8] = freq9;
    freqArray[9] = freq10;

    for (int x = 0; x < 10; x++)
        if(max < freqArray[x])
            max = freqArray[x];
    // this returns the integer number of the group 
    return max;

   //return arr[0].val;
} 





 // Driver code 
int classifierSystem(std::pair<double, double> gesture) 
{ 
    int n = 17; // Number of data points 
    Point arr[n]; 

    // to be replaced with the data you get 
    arr[0].x = 1; 
    arr[0].y = 12; 
    arr[0].val = 0; 
  
    arr[1].x = 2; 
    arr[1].y = 5; 
    arr[1].val = 0; 
  
    arr[2].x = 5; 
    arr[2].y = 3; 
    arr[2].val = 1; 
  
    arr[3].x = 3; 
    arr[3].y = 2; 
    arr[3].val = 1; 
  
    arr[4].x = 3; 
    arr[4].y = 6; 
    arr[4].val = 0; 
  
    arr[5].x = 1.5; 
    arr[5].y = 9; 
    arr[5].val = 1; 
  
    arr[6].x = 7; 
    arr[6].y = 2; 
    arr[6].val = 1; 
  
    arr[7].x = 6; 
    arr[7].y = 1; 
    arr[7].val = 1; 
  
    arr[8].x = 3.8; 
    arr[8].y = 3; 
    arr[8].val = 1; 
  
    arr[9].x = 3; 
    arr[9].y = 10; 
    arr[9].val = 0; 
  
    arr[10].x = 5.6; 
    arr[10].y = 4; 
    arr[10].val = 1; 
  
    arr[11].x = 4; 
    arr[11].y = 2; 
    arr[11].val = 1; 
  
    arr[12].x = 3.5; 
    arr[12].y = 8; 
    arr[12].val = 0; 
  
    arr[13].x = 2; 
    arr[13].y = 11; 
    arr[13].val = 0; 
  
    arr[14].x = 2; 
    arr[14].y = 5; 
    arr[14].val = 1; 
  
    arr[15].x = 2; 
    arr[15].y = 9; 
    arr[15].val = 0; 
  
    arr[16].x = 1; 
    arr[16].y = 7; 
    arr[16].val = 0; 
  
    /*Testing Point*/
    // The x and y value cannot be hardcoded, it has to be actively obtained from the gesture
    Point p; 
    p.x = 2.5; 
    p.y = 7; 
  
    // Parameter to decide group of the testing point 
    int k = 3; 
    printf ("The value classified to unknown point"
            " is %d.\n", classifyAPoint(arr, n, k, p)); 
    return 0; 
} 

std::map<String, std::pair<double, double>> gestureMap () {
    /*Library that maps each tuple of data to the corresponding gesture*/
    std::map<String, std::pair<double, double>> gestureLibrary;
    gestureLibrary['Hello'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['Goodbye'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['Nice_to_meet_you'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['Yes'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['No'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['Please'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['Thanks'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['1'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['2'] = std::pair<double, double>(0.0,0.0);
    gestureLibrary['3'] = std::pair<double, double>(0.0,0.0);
    return gestureLibrary;
}

std::map<String, int > gestureToInt () {
    /*Library that maps each tuple of data to the corresponding gesture*/
    std::map<String, int> gestureLibrary;
    gestureLibrary['Hello'] = 1;
    gestureLibrary['Goodbye'] = 2;
    gestureLibrary['Nice_to_meet_you'] = 3;
    gestureLibrary['Yes'] = 4;
    gestureLibrary['No'] = 5;
    gestureLibrary['Please'] = 6;
    gestureLibrary['Thanks'] = 7;
    gestureLibrary['1'] = 8;
    gestureLibrary['2'] = 9;
    gestureLibrary['3'] = 10;
    return gestureLibrary;
}

double distanceCalculator(std::pair<double, double> gestureData, std::pair<double, double> dataPoint){
    /* function that, given a gesture data and a co-ordinate, returns the distance*/
    double left_flex = gestureData.first;
    double right_flex = gestureData.second;
    double left_pt = dataPoint.first;
    double right_pt = dataPoint.second;
    double xDist = abs(left_flex - left_pt);
    double yDist = abs(right_flex - right_pt);
    double stretchDist = sqrt(pow(xDist, 2.0) + pow(yDist, 2.0));
    return stretchDist;
}

double gestureMatcher(std::map<String, std::pair<double, double>> gestureData, std::pair<double, double> dataPoint){
  double minimumBar = 0.0;
  for ( const auto &p : gestureData ){
     /* loop through each data point in the library and calulate its distance and then find minimum. */


  }


}