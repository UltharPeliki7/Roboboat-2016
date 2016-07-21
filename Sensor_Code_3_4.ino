#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

ros::NodeHandle nh;

sensor_msgs::Range range_msg;

///Instantiate a Publisher with the topic name of "ultrasound 3 or 4"
ros::Publisher pub_range3("ultrasound3", &range_msg);
ros::Publisher pub_range4("ultrasound4", &range_msg);

char frameid[] = "ultrasound";
int dist3=0;
int dist4=0;
  
/*
CODE HERE
Array of HC-SR04 ultrasonic sensors
*/

//Sonar 3
int echoPin3 =2;    // White Wire
int initPin3 =3;    // Grey Wire
int distance3 =0;

//Sonar 4
int echoPin4 =7;
int initPin4 =6;
int distance4 =0;

void setup(){
  nh.initNode();    ///Initialize ROS node handle
  
  /****** Advertise any of the Topics being Published*****/

  nh.advertise(pub_range3);
  nh.advertise(pub_range4);

  range_msg.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_msg.header.frame_id =  frameid;
  range_msg.field_of_view = 0.1; 
  range_msg.min_range = 0.0;
  range_msg.max_range = 6.47;
  

  pinMode(initPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(initPin4, OUTPUT);
  pinMode(echoPin4, INPUT);
 
  Serial.begin(57600);

  
  
}

void loop() {

  distance3 = getDistance(initPin3, echoPin3);
  printDistance(3, distance3);
  delay(150);
  dist3=distance3;
  distance4 = getDistance(initPin4, echoPin4);
  printDistance(4, distance4);
dist4=distance4;
  delay(150);
 
  range_msg.range = (getDistance(initPin3, echoPin3)+dist3)/2;
  range_msg.header.stamp = nh.now();
  pub_range3.publish(&range_msg); 
  
  range_msg.range = (getDistance(initPin4, echoPin4)+dist4)/2;
  range_msg.header.stamp = nh.now();
  pub_range4.publish(&range_msg);
 
  
  nh.spinOnce(); //Calls ROS.SpinOnce where all of the communication callbacks are handled
  
  
}

int getDistance (int initPin, int echoPin){

 digitalWrite(initPin, HIGH);
 delayMicroseconds(10); 
 digitalWrite(initPin, LOW); 
 unsigned long pulseTime = pulseIn(echoPin, HIGH);
 int distance = pulseTime/74/2;
 return distance;
 
}
 
 void printDistance(int id, int dist){
   
  /*   Serial.print(id);
   if (dist >= 120)
    {
      Serial.println(" Out of range");
    }
    
    else if ( dist <= 5)
    {
      Serial.println(" Object Too Close");
    }
    
    else 
    for (int i = 0; i <= dist; i++) 
    { 
         Serial.print("-");
    }*/
    
    Serial.print(dist, DEC);
    Serial.println(" in");
    
 }
