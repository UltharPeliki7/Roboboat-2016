#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

ros::NodeHandle nh;

sensor_msgs::Range range_msg;

ros::Publisher pub_range1("ultrasound1", &range_msg);   ///Instantiate a Publisher with the topic name of "ultrasound1"
ros::Publisher pub_range2("ultrasound2", &range_msg);


char frameid[] = "ultrasound";
int dist1=0;
int dist2=0;
  
/*
CODE HERE
Array of HC-SR04 ultrasonic sensors
*/


//Sonar 1
int echoPin1 =2;    // White Wire
int initPin1 =3;    // Grey Wire
int distance1 =0;

//Sonar 2
int echoPin2 =7;    // White Wire
int initPin2 =6;    // Grey Wire
int distance2 =0;


void setup()
{
  nh.initNode();    ///Initialize ROS node handle
  
  /****** Advertise any of the Topics being Published*****/
  
  nh.advertise(pub_range1);
  nh.advertise(pub_range2);

  range_msg.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_msg.header.frame_id =  frameid;
  range_msg.field_of_view = 0.1; 
  range_msg.min_range = 0.0;
  range_msg.max_range = 6.47;
  
 
  
  pinMode(initPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(initPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  Serial.begin(57600);

  
  
}

void loop() {
  
  distance1 = getDistance(initPin1, echoPin1); 
  printDistance(1, distance1);
  delay(150);
 dist1=distance1;
  distance2 = getDistance(initPin2, echoPin2);
  printDistance(2, distance2);
  dist2=distance2;
  delay(150);

  range_msg.range = (getDistance(initPin1, echoPin1)+dist1)/2;
  range_msg.header.stamp = nh.now();
  pub_range1.publish(&range_msg);    //Publishes Range 1 
  
  range_msg.range = (getDistance(initPin2, echoPin2)+dist2)/2;
  range_msg.header.stamp = nh.now();
  pub_range2.publish(&range_msg); 

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
