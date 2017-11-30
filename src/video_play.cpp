/*************************************************************************
*
*              Author: b51
*                Mail: b51live@gmail.com
*            FileName: video_play.cpp
*
*          Created On: Fri 29 Sep 2017 02:23:35 PM CST
*     Licensed under The MIT License [see LICENSE for details]
*
************************************************************************/

#include <ros/ros.h>

#include <termios.h>
#include <thread>
#include <chrono>

#include <opencv2/highgui/highgui.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include "sensor_msgs/Image.h"

int c = 0;

void getch(int n)
{
  while (ros::ok())
  {
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);           // save old settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON);                 // disable buffering
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // apply new settings

    c = getchar();  // read character (non-blocking)

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // restore old settings
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "vidoe2image");
  ros::NodeHandle nh;

  std::thread kb_thread(getch, 0);

  image_transport::ImageTransport it(nh);
  image_transport::Publisher image_pub;

  image_pub = it.advertise("/camera/rgb/image_raw", 1);

  cv::VideoCapture cap(argv[1]);
  if (!cap.isOpened())
    return -1;
  cv_bridge::CvImage cv_ptr;
  std::string title = std::string(argv[1]);
  //cv::namedWindow(title, 1);
  cv::Mat image;
  ROS_INFO("Playing");
  while (ros::ok())
  {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty())
      return(0);
    //ROS_INFO("%d, %d", frame.rows, frame.cols);

    cv_ptr.image = frame.clone();
    cv_ptr.encoding = "bgr8";
    //cv::imshow(title, image);
    //cv::imshow(title, cv_ptr.image);
    image_pub.publish(cv_ptr.toImageMsg());

    if (c == 32)
    {
      ROS_WARN("PAUSE");
      c = 0;
      while (true)
      {
        if (c == 32)
        {
          c = 0;
          ROS_WARN("CONTINUE");
          break;
        }
      }
    }
    else if (c == 0x71)
    {
      kb_thread.join();
      return 0;
    }
  }
  kb_thread.join();
  return 0;
}
