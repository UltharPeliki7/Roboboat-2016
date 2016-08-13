#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

ros::NodeHandle nh;

sensor_msgs::Range range_msg;

ros::Publisher pub_range5("Lidar", &range_msg);   ///Instantiate a Publisher with the topic name of "ultrasound1"
//ros::Publisher pub_range2("ultrasound2", &range_msg);


char frameid[] = "Lidar";

  unsigned long pulse_width;


void setup(){
  nh.initNode();    ///Initialize ROS node handle
  
  /****** Advertise any of the Topics being Published*****/
  
  nh.advertise(pub_range5);
  //nh.advertise(pub_range2);

  range_msg.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_msg.header.frame_id =  frameid;
  range_msg.field_of_view = 0.1; 
  range_msg.min_range = 0.0;
  range_msg.max_range = 6.47;
  
 
  
  pinMode(2, OUTPUT);                          // Set pin 2 as trigger pin
  pinMode(3, INPUT);                           // Set pin 3 as monitor pin
  digitalWrite(2, LOW);          // Set trigger LOW for continuous read
  Serial.begin(57600);    // Start serial communications

  
  
}

void loop() {



  //nh.spinOnce(); //Calls ROS.SpinOnce where all of the communication callbacks are handled

                       
    pulse_width = pulseIn(3, HIGH);            // Count how long the pulse is high in microseconds
    
                           
      pulse_width = pulse_width/10;          // 10usec = 1 cm of distance for LIDAR-Lite
      //  lidar_msg.LaserScan= pulse_width;
      range_msg.range = pulse_width;
      range_msg.header.stamp = nh.now();
      pub_range5.publish(&range_msg);
      Serial.println(pulse_width);           // Print the distance
    
    delay(400);                                   //Delay so we don't overload the serial port
  
nh.spinOnce(); //Calls ROS.SpinOnce where all of the communication callbacks are handled
  
  
  
}

