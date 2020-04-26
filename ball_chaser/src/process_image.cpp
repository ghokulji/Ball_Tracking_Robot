#include "ros/ros.h"
#include "iostream"
#include "ball_chaser/DriveToTarget.h"
#include<sensor_msgs/Image.h>
ros::ServiceClient client;

using namespace std;

void drive_robot(float lin_x, float ang_z)
{
	
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;
	ROS_INFO(" Velocities set");
	//client.call(srv);
	ros::service::waitForService("/ball_chaser/command_robot");
	if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}



void process_image_callback( const sensor_msgs::Image img)
{
	
	int white_pixel = 255;
	float linear_x, angular_z;
	int counter = 0;

        //I actually did this knowingly, I went through the documentation before writing this, I thought that each pixel can be represented by the columns thats why I used img.width instead of img.step/3.
	float one = img.width;
	float pixel_finder = 0;	

	for(int i=0; i< (img.height * img.step) ; i += 3)
	{
			//RGB-3 bytes, all bytes should be 255
			if((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255))
			{	
				ROS_INFO("OK1");
				pixel_finder = i % img.step;
				counter = 1;
				break;
			}

	}


	if(counter == 1)
		{
		ROS_INFO("OK2");
		if(pixel_finder <= one)
					{
							ROS_INFO("LEFT");	
							linear_x = 0.5;
							angular_z = 0.8;
							drive_robot(linear_x,angular_z);
					}

		else if(pixel_finder <= (one*2))
					{
						ROS_INFO("FORWARD");	
						linear_x = 0.5;
						angular_z = 0.0;
						drive_robot(linear_x,angular_z);
					}

		else if(pixel_finder <= (one*3))
					{
						ROS_INFO("RIGHT");	
						linear_x = 0.5;
						angular_z = -0.8;
						drive_robot(linear_x,angular_z);
					}

		}
			
	else
	{
		ROS_INFO("NO BALL");
		drive_robot(0,0);
	}
	
		
}

int main( int argc, char** argv)
{

	ros::init(argc, argv, "process_image");
	ros::NodeHandle n;

	 client = n.serviceClient<ball_chaser::DriveToTarget>("ball_chaser/command_robot");

	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw",10, process_image_callback);

	ros::spin();
	return 0;
}
