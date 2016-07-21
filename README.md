# Roboboat -2016
This year we embark upon a great journey
Our intent here is to integrate sophisticated machine learning algorithms with existing computer vision software to complete the obstacle avoidance, pattern recognition, and various other task requirements of the 2017 Roboboat competition.

Onboard sensors currently installed:
Lidar on a servo that can pan in the xy plane in a 180 degree arc WRT the boat
1080p Webcam in front with a 90 degree FoV
4 Ultrasonic sensors
A GPS -3DR Robotics Module
Magnetometer hmc5883l

Sensors at disposal:
2 Ultrasonic sensors
3 Cameras
2 IMUs

Completed:
Lidar refresh rate: Currently <100 readings per second, needs to be converted to >500 per second for optimal scan speeds
At 700+ per second


To Be Completed:
Gps/Magnetometer: Magnetometer code currently not working, arduino atmega board is suspect, signal is reading(incorrectly) for a short period then cutting off.
OpenCV: Currently not integrated into the navigation file. Works based on color detection and a best fit circle, and a set buoy size. Will misread larger buoys as being closer than they actually are.
Ultrasonic sensor: Currently not being used very well, positioning on US1 & US2 could be moved further forward, and US3 & US4 could be moved further backward to aid in obstacle avoidance. Reading angle unknown, ideally would be improved upon.
Changing message types to those compatible in Rviz
Increase understanding of network communication with boat and command center
Create a "webserver" on the boat that will broadcast what the boat is thinking


Future plans:
If boat is to be kept, installation of a gyroscopic stabilizer should be mandatory.
New boat to prevent the excessive pitching and rolling that the boat is susceptible to due to the wavebreaker design.
Artificial Neural Network/Machine learning approach to completing tasks
Learning OpenCV/getting someone to go from solely color detection to edge detection(could be remedied by ANN). Harsh light/reflections and worn buoys pose significant problems while identifying buoys
