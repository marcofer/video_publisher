#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream> // for converting the command line parameter to integer
#include <iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/gpu/gpu.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  cout << cv::getBuildInformation() << endl;
  /*if(argc != 2){
    cout << "Usage: \n rosrun rawVideoPub video_publisher_node [video source file]";
    return 1;
  }//*/

  string filename, topicname;
  filename = string(argv[1]);
  topicname = string(argv[2]);

  // Check if video source has been passed as a parameter
  //if(argv[1] == NULL) return 1;

  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  

  image_transport::Publisher pub = it.advertise(topicname, 100);

  double publishing_rate;

  if(nh.getParam("publishing_rate",publishing_rate))
      ROS_INFO("publishing_rate set to %f ", publishing_rate);
  else publishing_rate = 30.0;


  cout << "Number of cores: " << sysconf( _SC_NPROCESSORS_ONLN ) << endl;

  cv::VideoCapture cap(filename);

  if(!cap.isOpened()){ 
    cout << "File not found." << endl;
    return 1;
  }


  cv::Mat frame;
  sensor_msgs::ImagePtr msg;
  ros::Rate loop_rate(publishing_rate);

  int frame_counter = 0;

  while (nh.ok()) {

    try{
        cap >> frame;
    }
    catch(...){

        std::cout << "Error in cap >> frame instruction. " << std::endl;
        return 1;
    }


    frame_counter ++;

    if (frame_counter == cap.get(CV_CAP_PROP_FRAME_COUNT)){
      frame_counter = 0;
      cap.set(CV_CAP_PROP_POS_FRAMES,0);
    }

    // Check if grabbed frame is actually full with some content
    if(!frame.empty()) {
      //cout << "size frame: " << frame.size() << endl;
      resize(frame,frame,cv::Size(320,240));
      msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
      pub.publish(msg);
      imshow("frame",frame);
      cv::waitKey(30);
    }

    ros::spinOnce();
    loop_rate.sleep();
  }
}
