#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

#include <bram_uio.h>
#include <xnn_inference.h>
#include <xparameters.h>


BRAM bram0(0,64000);

XNn_inference neuralNet;

const int target_width = 32;
const int target_height = 18;

const char* AI_name = "al";

class ImageSubscriber : public rclcpp::Node
{
	public:
		ImageSubscriber() : Node("image_subscriber") {
			RCLCPP_INFO(this->get_logger(), "Initializing ImageSubscriber node");

			RCLCPP_INFO(this->get_logger(), "Starting camera subscription");

			camera_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
					"/image_raw",
					10,
					std::bind(&ImageSubscriber::onImageMsg, this, std::placeholders::_1)
			);
    		x_status = XNn_inference_Initialize(&neuralNet, AI_name);
_			

		}

	private:
		rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscription_;

		void onImageMsg(const sensor_msgs::msg::Image::SharedPtr msg) {
			RCLCPP_INFO(this->get_logger(), "Received image!");

			cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
			cv::Mat img = cv_ptr->image;

			cvtColor(img, gray, COLOR_BGR2GRAY);

			Mat resized;
			cv::resize(gray, resized, Size(target_width, target_height));
			int k = 0;
			for (int i = 0; i < target_height; i++)
			{
				for (int j = 0; j < target_width; j++)
				{
					bram0[k] = resized(j,i);
					k++;					
				}
				
			}

			XNn_inference_Start(&neuralNet);			


			RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
		}

};

int main(int argc, char *argv[])
{
	setvbuf(stdout,NULL,_IONBF,BUFSIZ);

	rclcpp::init(argc,argv);
	rclcpp::spin(std::make_shared<ImageSubscriber>());

	rclcpp::shutdown();
	return 0;
}
