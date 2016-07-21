
#include <Servo.h>
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#define USE_USBCON



ros::NodeHandle nh;                          //Instantiate the Node Handle******

int LT;
int RT;


void LmessageCb(const std_msgs::Int16& msg)
{
  LT = msg.data;
}
void RmessageCb(const std_msgs::Int16& msg)
{
  RT = msg.data;
}

std_msgs::Int16 left;
std_msgs::Int16 right;

ros::Subscriber<std_msgs::Int16> L("Left", &LmessageCb);
ros::Subscriber<std_msgs::Int16> R("Right", &RmessageCb);

char var = 's';
//switched pins
byte leftmotorPin = 9;                      // Set Pin 9 to the output to the ESC (speed controller)
byte rightmotorPin = 7;                     // Set Pin 7 to the output to the ESC (speed controller)
int ch2 = 5;                                 // set the value coming from channel 1 of the receiver to Pin 5 (Right Throttle)
int ch3 = 3;                                // set the value coming from channel 3 of the receiver to Pin 3 (Left Throttle)
int ch5 = 13;                               // set the value coming from channel 5 of the receiver to Pin 13 (Mode Switch)
int userValue = 0; 
int LM = 0; 
int RM = 0; 
int LMtoESC = 1500; 
int RMtoESC = 1500;
int idle = 1500;


Servo leftmotor;                            // declare the left motor output variable
Servo rightmotor;                           // declare the right motor output variable
int lastLMspeed = 0;                     // Initialize the last speed of the Left motor as 1500
int lastRMspeed = 0;                     // Initialize the last speed of the Right motor as 1500
int LMspeed = 1500;
int RMspeed = 1500; 


void setup() 
{
        nh.initNode();                           //Initialize the Node Handle*****
        nh.subscribe(L);                       //Advertise any topic being published*******
        nh.subscribe(R);                       //Advertise any topic being published*******
        
        Serial.begin(57600);                    // Begin Serial communication with the computer
        pinMode(ch2, INPUT);                     // Set Pin for ch2 as input  
        pinMode(ch3, INPUT);                     // Set Pin for ch3 as input
        pinMode(ch5, INPUT);                   // Set Pin for ModePin as an Input  
	
        leftmotor.attach(leftmotorPin);          // Attach Left motor output to Pin 9
        rightmotor.attach(rightmotorPin);        // Attach Right motor output to Pin 7

	leftmotor.writeMicroseconds(1500);       // send "stop" signal to ESC.
        rightmotor.writeMicroseconds(1500);      // send "stop" signal to ESC.
	delay(500);                             // delay to allow the ESC to recognize the stopped signal and initialize
      
}


/*
/************ First step in the loop is to determine what mode the vessel is operating in according to the button that is on the vessel********
*/

void loop()
{
 
  userValue = map(pulseIn(ch5, HIGH),1200,1800,10,-10);                  // Read input value  of the Mode switch
  
  if (userValue > 0) {      /// If mode switch is > 0 
  
  left.data = LT;
  right.data = RT;
  
  leftmotor.writeMicroseconds(LT);
  rightmotor.writeMicroseconds(RT);
  
   Serial.println("Autonomous Mode");   // Print Autonomous mode so we know what mode it is in
   Serial.println(" ");
   Serial.println(LT);
   Serial.println(RT);
   Serial.println(" ");
    nh.spinOnce();
    delay(100);
   }
/*
/ **********This begins the point of the code for using the remote control to control the vessel************
*/

    else {                                                          //If mode switch is 0 means the vessel is in remote control mode
       LM = map(pulseIn(3, LOW), 17500, 18800, 1100, 1900);          // Read the value from the left joystick of the remote control
        RM = map(pulseIn(5, LOW), 17500, 18800, 1100, 1900);         // Read the value from the right joystick of the remote control
      
      if(lastLMspeed!=0 && lastRMspeed!=0)                           //If there is no value for lastLMspeed or lastRMspeed meaning it has just gone into Remote Control mode
         {
               LMtoESC = LM + lastLMspeed;                          // Add last left thruster speed to the speed coming from the Remote Control
               RMtoESC = RM + lastRMspeed;                          // Add last right thruster speed to the speed coming from the Remote Control
           
               LMtoESC = LMtoESC / 2;                               // Take the average of the last left thruster speed and the current value coming from the Remote Control
         
               RMtoESC = RMtoESC / 2;                               // Take the average of the last right thruster speed and the current value coming from the Remote Control
               
               LMtoESC = LMtoESC + 1500;                            // Add 1500 to the left Average calculated in the last step to get the values within the range accepted by the ESC
               RMtoESC = RMtoESC + 1500;                            // Add 1500 to the right Average calculated in the last step to get the values within the range accepted by the ESC
             }
      
        lastLMspeed = LM;                                          // Set the left speed from the Remote Control as the last left thrust speed
        lastRMspeed = RM;                                          // Set the right speed from the Remote Control as the last left thrust speed
         
       if(LMtoESC >= 1460 && LMtoESC <= 1540 && RMtoESC >= 1460 && RMtoESC <= 1540)                  // Checks the value of the Left speed and determines if it is within the idle range
  {
    leftmotor.writeMicroseconds(idle);                    // Send signal to ESC for left motor
    rightmotor.writeMicroseconds(idle);
        
  }
  
  else if((LMtoESC >= 1460 && LMtoESC <= 1540) && (RMtoESC < 1460 || RMtoESC > 1540))                  // Checks the value of the Left speed and determines if it is within the idle range
  {
    leftmotor.writeMicroseconds(idle);                   // Send signal to ESC for left motor
    rightmotor.writeMicroseconds(RMtoESC);
   
    
  }
  
  else if((RMtoESC >= 1460 && RMtoESC <= 1540) && (LMtoESC < 1460 || LMtoESC > 1540))                  // Checks the value of the Left speed and determines if it is within the idle range
  {
    rightmotor.writeMicroseconds(idle);                   // Send signal to ESC for left motor
    leftmotor.writeMicroseconds(LMtoESC);
    
  }
  
  else
  {
     leftmotor.writeMicroseconds(LMtoESC);
     rightmotor.writeMicroseconds(RMtoESC);
   
  }
  
  Serial.println(LMtoESC);
  Serial.println(RMtoESC);
  Serial.println(" ");

                        
     delay(100);
 }
}

