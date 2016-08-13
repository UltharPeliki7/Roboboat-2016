/* =============================================================================
  LIDAR-Lite v2: Single sensor, get distance as fast as possible

  This example file demonstrates how to take distance measurements as fast as
  possible, when you first plug-in a LIDAR-Lite into an Arduino it runs 250
  measurements per second (250Hz). Then if we setup the sensor by reducing the
  aquisiton record count by 1/3 and incresing the i2c communication speed from
  100kHz to 400kHz we get about 500 measurements per second (500Hz). Now if we
  throttle the reference and preamp stabilization processes during the distance
  measurement process we can increase the number of measurements to about 750
  per second (750Hz).

   The library is in BETA, so subscribe to the github repo to recieve updates, or
   just check in periodically:
   https://github.com/PulsedLight3D/LIDARLite_v2_Arduino_Library

   To learn more read over lidarlite.cpp as each function is commented
=========================================================================== */

#include <Wire.h>
#include <LIDARLite.h>
#include <Servo.h> 
 int pan=1;
Servo myservo;
// Create a new LIDARLite instance
LIDARLite myLidarLite;
int temp=0;
int maximum=0;
int minimum=5000;
int prev;
int pos=0;
void setup() {
  Serial.begin(115200);
myservo.attach(7);
  //  First we want to set the aquisition count to 1/3 the default (works great for stronger singles)
  //  can be a little noisier (this is the "1"). Then we set the "true" to enable 400kHz i2c
  //  communication speed.

  myLidarLite.begin(1,true);
  Serial.println(myLidarLite.distance());
}

void loop() {
 for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    Serial.println(myLidarLite.distance(false,false));
    delay(pan);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    Serial.println(myLidarLite.distance(false,false));
    delay(pan);                       // waits 15ms for the servo to reach the position 
  } 
  //  Next we need to take 1 reading with preamp stabilization and reference pulse (these default to true)
  /*
myservo.write(150);
  // Next lets take 99 reading without preamp stabilization and reference pulse (these read about 0.5-0.75ms faster than with)
  for(int i = 0; i < pan; i++){
    temp=(myLidarLite.distance(false,false)+prev)/2;
    if(temp>maximum)
    {maximum=temp;
    Serial.println(maximum);
    }
    
    
    if(temp<minimum)
    {minimum=temp;
    Serial.println(minimum);
    }
    prev=temp;
  }
  myservo.write(30);
    for(int i = 0; i < pan; i++)
    {temp=(myLidarLite.distance(false,false)+prev)/2;
    if(temp>maximum)
    {maximum=temp;
    Serial.println(maximum);
    }
    if(temp<minimum)
    {minimum=temp;
    Serial.println(minimum);
    }
    prev=temp;
    //Serial.println(myLidarLite.distance(false,false));
    }*/
}
