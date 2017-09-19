#include <ros/ros.h>

#include <opencv2/highgui/highgui.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include "sensor_msgs/Image.h"

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "exporsure_check");
  ros::NodeHandle nh;

  image_transport::ImageTransport it(nh);
  image_transport::Publisher image_pub;

  image_pub = it.advertise("/camera/rgb/image_rect_color", 1);

  cv::VideoCapture cap(argv[1]);
  if (!cap.isOpened())
    return -1;
  cv_bridge::CvImage cv_ptr;
  std::string title = std::string(argv[1]);
  cv::namedWindow(title, 1);
  cv::Mat image;
  while (ros::ok())
  {
    cv::Mat frame;
    cap >> frame;
    //ROS_INFO("%d, %d", frame.rows, frame.cols);
    cv_ptr.image = frame.clone();
    //cv::imshow(title, image);
    //cv::imshow(title, cv_ptr.image);
    //cv::waitKey(1);
    image_pub.publish(cv_ptr.toImageMsg());
  }
  return 0;
}
