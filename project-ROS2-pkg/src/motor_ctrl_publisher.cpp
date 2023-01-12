#include <chrono>
#include <functional>
#include <memory>
#include <string> 
#include <iostream>
#include <cmath>

#include <rclcpp/rclcpp.hpp>
#include <msg/set_position.hpp>

#include <../drivers/nn_inference_v1_0/xnn_inference.h>

using namespace std::chrono_literals;

float x,y = 0.0;
float [] invres = {0.0,0.0};
XNn_Inference NN;



class MotorPublisher : public rclcpp::Node
{   
    public:
        MotorPublisher():Node("motor_publisher"), count_(0)
        {
            publisher_ = this->create_publisher<dynamixel_sdk_custom_interfaces::msg::SetPosition>("motor_ctrl",10);
            timer_ = this->create_wall_timer(
                500ms, std::bind(&MotorPublisher::timer_callback, this));    
            
            XNn_inference_Initialize(&NN,'ai');
        }

    private:

    float* inverse(float x, float y)
    {
        float l1 = 1;
        float l2 = 1;
        float d = 2*l1*l2;
        float n = (x*x+y*y)-(l1*l1-l2*l2);
        float o1 =  atan2(y,x) * (180/M_PI);
        float o2 = acos(n/d) * (180/M_PI);
        
        float myNum[] = {o1,o2};
        return myNum;
    }

    void timer_callback()
    {
      //auto message = std_msgs::msg::String();
      //message.data = "Hello, world! " + std::to_string(count_++);gitclone-b $ROS_DISTRO-develhttps://github.com/ROBOTIS-GIT/DynamixelSDK.git
      //RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      //publisher_->publish(message);

      auto message = dynamixel_sdk_custom_interfaces::msg::SetPosition();

      std::string joint_1 = '2';
      std::string joint_2 = '3';

      float angle_1 = 0.0;
      float angle_2 = 0.0;

      int pose = 0;

      while(!XNn_inference_IsDone(&NN)){}

      pose = (int)XNn_inference_Get_return(&NN);
      
      switch (pose)
      {
      case 0: // 
        /* code */
        x = 2*sqrt(2);
        y = 2*sqrt(2); 
        invres = inverse(x,y);
        break;
      case 1:
        x = 1.00;
        y = 1.00;
        invres = inverse(x,y);
        break;
      case 2:
        x = 0.0;
        y = 2.0;
        invres = inverse(x,y);
        break;
      case 3:
        x = 1.00;
        y = 1.00
        invres = inverse(x,y);
        break;
      case 4:
        x = 2.0;
        y = 0.0;
        invres = inverse(x,y);
        break;  
      default:
        break;
      }

      // publish message for first joint
      message.data = joint_1 + std::to_string(invres[0]);
      publisher_->publish(message);

      // publish message for second joint
      message.data = joint_2 + std::to_string(invres[1]);
      publisher_->publish(message);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<dynamixel_sdk_custom_interfaces::msg::SetPosition>::SharedPtr publisher_;
    size_t count_;
};
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotorPublisher>());
  rclcpp::shutdown();
  return 0;
}
