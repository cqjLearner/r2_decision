#include "ros/ros.h"
#include "r2_msgs/decision.h"
int main(int argc, char *argv[])
{
    ros::init(argc,argv,"test");
    ros::NodeHandle nh;
    // ros::Publisher pub = nh.advertise<std_msgs::Int8>("test",1);
    // int num = 1;
    // nh.getParam("num",num);
    ros::Publisher pub_decide = nh.advertise<r2_msgs::decision>("test",1);
    r2_msgs::decision d;
    d.silo_num = 1;
    d.competition_status = 0;
    ros::Rate r(10);
    
    while(ros::ok())
    {
        r.sleep();
        // ROS_INFO("This is %d",num);
        pub_decide.publish(d);
    }
    return 0;
}
