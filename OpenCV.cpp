#define _USE_MATH_DEFINES
#include <cmath>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#define inDataSize 256
#include <iostream>
#define MAXIMUM_TEXT_SIZE 64u
#include <math.h>       /* atan2 */
#define input "video"
#define show false

#define minPix 20 //minimum pixel size
#define minDist 15 //distance in inches to stop/go
int counter = 0;


//struct to hold HSV values; makes coding cleaner and easier
struct colorStruct {
    std::string name; const char* tempName; //name holders
    int hLow; int hHigh; int sLow; int sHigh; int vLow; int vHigh;//color values
    cv::Scalar color;//color to display for this entity
    std::vector<cv::Point> contour; // Vector for storing contour if found
    int xEstimate = 0;
    int yEstimate = 0;
    float angle;
    float distance; //why not store it here; note: if error initialize under constructor
                    //constructor with given formal parameters; trust user to use this
    colorStruct(std::string Name, int Hlow, int Hhigh, int Slow, int Shigh, int Vlow, int Vhigh, cv::Scalar Color) {
        name = Name; hLow = Hlow; hHigh = Hhigh; sLow = Slow; sHigh = Shigh; vLow = Vlow; vHigh = Vhigh; color = Color; distance = -1.0;
    }
    //function to create a trackbar to adjust HSV
    void createTrackBar() {
        tempName = (name + " Trackbar").c_str();
        cvNamedWindow(tempName);
        //cvResizeWindow  (tempName, 350, 350);
        cvCreateTrackbar("H-Low", tempName, &hLow, 255, NULL);//these are the trackbars...
        cvCreateTrackbar("H-High", tempName, &hHigh, 255, NULL);
        cvCreateTrackbar("S-Low", tempName, &sLow, 255, NULL);
        cvCreateTrackbar("S-High", tempName, &sHigh, 255, NULL);
        cvCreateTrackbar("V-Low", tempName, &vLow, 255, NULL);
        cvCreateTrackbar("V-High", tempName, &vHigh, 255, NULL);
    }
};

//function to thresh image using HSV information from struct
//returns threshed image
cv::Mat threshHSV(cv::Mat img, colorStruct colorHSV) {
    cv::Mat imgThreshedHSV;
    cvtColor(img, imgThreshedHSV, cv::COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
                                                      //inputImage,scalar(hlow,slow,vlow),scalar(hhigh,shigh,vhigh),outputImage
    inRange(imgThreshedHSV, cv::Scalar(colorHSV.hLow, colorHSV.sLow, colorHSV.vLow),
        cv::Scalar(colorHSV.hHigh, colorHSV.sHigh, colorHSV.vHigh), imgThreshedHSV); //Threshold the image in range
                                                                                     //morphological opening (remove small objects from the foreground)
    erode(imgThreshedHSV, imgThreshedHSV, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));//<-kernel size
    dilate(imgThreshedHSV, imgThreshedHSV, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    //morphological closing (fill small holes in the foreground)
    dilate(imgThreshedHSV, imgThreshedHSV, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    erode(imgThreshedHSV, imgThreshedHSV, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    return imgThreshedHSV;
}

//find the max contour of the given color
std::vector<cv::Point> findMaxContour(cv::Mat img, cv::Mat imgThreshed, colorStruct& light) {
    std::vector< std::vector<cv::Point> > contours; // Vector for storing contour
    std::vector<cv::Point> blank;
    std::vector<cv::Vec4i> hierarchy;
    int largest_area = 0;
    int largest_contour_index = -1;
    cv::Rect bounding_rect;
    // Find the contours in the image
    cv::findContours(imgThreshed, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i< contours.size(); i++) { // iterate through each contour.
        double a = contourArea(contours[i], false);  //  Find the area of contour
        if (a>largest_area) {
            largest_area = a;
            largest_contour_index = i;                //Store the index of largest contour
        }
    }
    if (largest_contour_index>-1)
        return contours[largest_contour_index];//return largest contour
    else
        return blank;
}

//find block height/width or distance
float findDist(cv::Mat& img, colorStruct& light) {

    cv::Mat lookUpX = cv::imread("/home/oduasv/Desktop/Test1080/xEst.bmp");
    cv::Mat lookUpY = cv::imread("/home/oduasv/Desktop/Test1080/yEst.bmp");

    float distance = -1.0; //use as control
                           //get rectangle box in order to find width and height regardless of angle
    cv::RotatedRect box = minAreaRect(light.contour);
    cv::Point2f vtx[4];
    box.points(vtx);
    //width
    float x = sqrt(pow((vtx[1].x - vtx[0].x), 2) + pow((vtx[1].y - vtx[0].y), 2));
    //height
    float y = sqrt(pow((vtx[2].x - vtx[1].x), 2) + pow((vtx[2].y - vtx[1].y), 2));

    if(1){
        cv::Point2f center;
        float radius = 0;
        cv::minEnclosingCircle(light.contour, center, radius);
        //find distance
        // distKnown/radiusKnown=distance/radiusFound
        //->distance = radiusFound*distKnown/radiusKnown = radiusFound*ratioKnown

//***************************************
        distance = 2800 * pow(radius, -1.027); //OLD ONE
      

        cv::circle( img, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );

        //draw circle around contour
        cv::circle(img, center, cvRound(radius), light.color, 1, CV_AA);

        int d = int(radius*2);
        int w = int(abs(center.x-960));
        if(d>=350){
            d=349;
        }
        if(w>=920){
            w=919;
        }
        int estimateX =int(lookUpX.at<uchar>(d,w,0));
        int estimateY =int(lookUpY.at<uchar>(d,w,0));

        if ((center.x -960)<0){
            light.xEstimate = -estimateX;
        } else{
            light.xEstimate = estimateX;
        }
        light.yEstimate = estimateY;
        distance = sqrt(estimateX*estimateX+estimateY*estimateY);
       

        //create string for output to image
        char text_array[MAXIMUM_TEXT_SIZE];
        snprintf(text_array, MAXIMUM_TEXT_SIZE, "%4.2f", estimateY);
        std::string temp(text_array);

        snprintf(text_array, MAXIMUM_TEXT_SIZE, "%4.2f", estimateX);
        std::string temp2(text_array);


    }

    if ((x + y) / 4>minPix)
        return distance;
    else
        return -1;
}

//find a sized contour; used for config
bool findSizedContour(cv::Mat& img, cv::Mat imgThreshed, colorStruct light) {
    std::vector< std::vector<cv::Point> > contours; // Vector for storing contour
    std::vector<cv::Vec4i> hierarchy;
    int largest_area = 0;
    int largest_contour_index = 0;
    cv::Rect rect;
    bool found = false;
    cv::findContours(imgThreshed, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
    for (int i = 0; i< contours.size(); i++) { // iterate through each contour.
        rect = boundingRect(contours[i]);
        if (((rect.height + rect.width) / 4)>minPix) {
            cv::rectangle(img, rect, light.color, 2, 8, 0);
            int sizeofcontour = (rect.width + rect.height) / 2;
            if (show)
                std::cout << light.name << " size=" << sizeofcontour << std::endl;
            found = true;
        }
    }
    return found;
}

//function to find and classify lights and return 'w' or 's'
std::string classifyStopLight(cv::Mat img, std::vector<colorStruct>& lights) {
    cv::Mat threshedImg;
    std::string botCmd;

    for (int i = 0; i<lights.size(); i++) {

        threshedImg = threshHSV(img, lights[i]);
        if (show)
            imshow("Thresholded Image " + lights[i].name, threshedImg); //show the thresholded image
                                                                        //method contours
        lights[i].contour = findMaxContour(img, threshedImg, lights[i]);
        if (lights[i].contour.size()>0) {
            lights[i].distance = findDist(img, lights[i]);
            cv::imshow("Buoy Detection", img); //show the boxed image

            //////////////////////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////output
            double distanceEstimate =0;
            double angleEstimate = 0;
            if (lights[i].xEstimate!=0 && lights[i].yEstimate!=0){
                distanceEstimate = sqrt(lights[i].xEstimate*lights[i].xEstimate+lights[i].yEstimate*lights[i].yEstimate);
                angleEstimate = atan((double)(lights[i].xEstimate)/(double)(lights[i].yEstimate))*180.0/3.14159265;
                if(i==0){
                    std::cout<<"RED"<<std::endl;
                } else if (i==1){
                    std::cout<<"GREEN"<<std::endl;
                }
                std::cout<<"distance  => "<<distanceEstimate<<std::endl;
                std::cout<<"angle  => "<<angleEstimate<<std::endl;
             }
             /////////////////////////////////////////////////////////////////////////////////////////////////
             ////////////////////////////////////////////////////////////////////////////////////////////////


        }
    }

    if ((lights[0].distance>0) && (lights[0].distance<minDist)) {
        counter = counter + 1;

    }
    if (counter > 7)
    {
        counter = 0;
        return "s";
    }

    else if((lights[1].distance>0)&&(lights[1].distance<minDist)){

    }



    return botCmd;
}

//function to adjust colors
//keyboard "c" activates function
//keyboard "n" selects next color on loop
//keyboard "esc" exits config and goes back to main program
void configMode(std::vector< colorStruct>& lights, cv::VideoCapture cap)
{
    int key = 0; int i = 0; bool looped = false; cv::Mat img; cv::Mat tempImg;
    while (key != 27) {
        lights[i].createTrackBar();
        while (true) {
            if (input != "video")
                img = cv::imread(input);
            else
                cap.read(img);
            cv::imshow("Original Image", img);
            tempImg = threshHSV(img, lights[i]);
            cv::imshow("Threshed " + lights[i].name, tempImg);
            findSizedContour(img, tempImg, lights[i]);
            cv::imshow("Buoy Detection", img);
            key = cv::waitKey(1);
            if (key == 27) { //wait for 'esc' key press . If 'esc' key is pressed, break loop
                std::cout << "Exit config mode." << std::endl;
                cv::destroyAllWindows();
                break;
            }
            else if (key == 110) {
                // close the windows
                cv::destroyAllWindows();
                i++;
                if (i >= lights.size()) {
                    i = 0;
                }
                break;
            }
        }
    }
}

int main()
{

    //create controls for keyboard use in while loop
    int key=0;
    cv::Mat img;//Setup image container
    cv::namedWindow("Original Image", CV_WINDOW_AUTOSIZE); //create a window

                                                           //create colorStruct vector to hold HSV colors
    std::vector< colorStruct > lights;

      //Red high level
      lights.push_back(colorStruct ("redhigh",165,255,146,255,98,255,cv::Scalar(0,0,255)));

    //Green
    lights.push_back(colorStruct("green", 37, 100, 100, 255, 77, 190, cv::Scalar(0, 255, 0)));



          //Setup video camera
        cv::VideoCapture cap(0); //capture the video from web cam
        if (!cap.isOpened()) {  // if not success, exit program
            std::cout << "Cannot open the web cam" << std::endl;
            return -1;//without webcam then useless->exit main
        }
        cap.set(3,1920);
        cap.set(4,1080);

        //define size of data
        int outDataSize = 1;
        //string to send command to bot
        std::string botCmd;


        //capture and detect until exit key
        while (true) {//could put key!=to some value here
                      //read a new frame from video
            if (!cap.read(img)) { //if not success, continue loop
                std::cout << "Cannot read a frame from video stream" << std::endl;
                continue;
            }
        
            imshow("Original Image", img); //show the original image
           botCmd = classifyStopLight(img, lights);//detect and return boxed lights with

                                                    //keyboard key use put here
            key = cv::waitKey(1);
            if (key == 27) { //wait for 'esc' key press . If 'esc' key is pressed, break loop
                std::cout << "esc key is pressed by user" << std::endl;
                break;
            }
            else if (key == 99) {
                std::cout << "Config Mode" << std::endl;
                configMode(lights, cap);
            }
          
        }


    return 0;
}
