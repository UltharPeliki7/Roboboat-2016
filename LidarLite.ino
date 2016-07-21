  
  /*
  *
  Servo Wiring Guide
  White = Signal
  Red = + (5V)
  Black = - (GND)
  *
  */
  
#include <Servo.h>
#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>
#include <Wire.h>
#include <LIDARLite.h>

// Create a new LIDARLite instance
LIDARLite myLidarLite;
ros::NodeHandle nh;

sensor_msgs::Range range_msg;
ros::Publisher lidar("Lidar", &range_msg);


char frameid[] = "lidar";



int servoPin= 8;

Servo servo;                                  // create servo object to control a servo
  

int angle = 0;                                    // variable to store the servo position

unsigned long pulse_width;

void setup()
{
  nh.initNode();                               //Initialize ROS node handle
  
  nh.advertise(lidar);

  range_msg.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_msg.header.frame_id =  frameid;
  range_msg.field_of_view = 0.1; 
  range_msg.min_range = 0.0;
  range_msg.max_range = 6.47;
   
  
  servo.attach(servoPin);                           // attaches the servo on pin 9 to the servo object
                      
  pinMode(2, OUTPUT);                          // Set pin 2 as trigger pin
  pinMode(3, INPUT);                           // Set pin 3 as monitor pin
  digitalWrite(2, LOW);          // Set trigger LOW for continuous read
    Serial.begin(115200);    // Start serial communications
    myLidarLite.begin(1,true);
}

void loop()
{/*
  pulse_width = pulseIn(3, HIGH);             // Count how long the pulse is high in microseconds
    if(pulse_width != 0)                       // If we get a reading that isn't zero, let's print it
    {                        
        pulse_width = pulse_width/10;          // 10usec = 1 cm of distance for LIDAR-Lite
       // Serial.println(pulse_width);           // Print the distance
       
       if(pulse_width>40)
       {
        range_msg.range = pulse_width;
        range_msg.header.stamp = nh.now();
        range_msg.field_of_view=angle;
        lidar.publish(&range_msg);
         Serial.println(pulse_width);           // Print the distance
  
  Serial.println("");
    }}
    
     //delay(1);    
   for(angle = 30; angle < 150; angle++)         // goes from 0 degrees to 180 degrees in steps of 2 degrees
   {          
    servo.write(angle);                        // tell servo to go to position in variable 'pos'
    //delay(15);                                // waits 15ms for the servo to reach the position
   pulse_width = pulseIn(3, HIGH);             // Count how long the pulse is high in microseconds
    if(pulse_width != 0)                       // If we get a reading that isn't zero, let's print it
    {                        
        pulse_width = pulse_width/10;          // 10usec = 1 cm of distance for LIDAR-Lite
       // Serial.println(pulse_width);           // Print the distance
        if(pulse_width>90)
      {
        range_msg.range = pulse_width;
        range_msg.header.stamp = nh.now();
        range_msg.field_of_view=angle;
        lidar.publish(&range_msg);
        
        delay(20); 
    }
  }
    
                                    //Delay so we don't overload the serial port
     
   }


  for(angle = 150; angle > 30; angle--)           // goes from 180 degrees to 0 degrees in steps of 2 degrees
  {        
    servo.write(angle);                        // tell servo to go to position in variable 'pos'
    //delay(15);                               // waits 15ms for the servo to reach the position
    pulse_width = pulseIn(3, HIGH);            // Count how long the pulse is high in microseconds
    if(pulse_width != 0)                       // If we get a reading that isn't zero, let's print it
    {                        
        pulse_width = pulse_width/10;          // 10usec = 1 cm of distance for LIDAR-Lite
      //  lidar_msg.LaserScan= pulse_width;
      if(pulse_width>90)
      {
       range_msg.range = pulse_width;
       range_msg.field_of_view=angle;
        range_msg.header.stamp = nh.now();
        lidar.publish(&range_msg);
        delay(20);
      }
  nh.spinOnce(); //Calls ROS.SpinOnce where all of the communication callbacks are handled
  
       
    
    
                                         //Delay so we don't overload the serial port
    }*/
    nh.spinOnce();
  

}

}
