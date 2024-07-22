#include "r2_decision/operation/TakeBall.h"

BT::NodeStatus r2_decision::TakeBall::onStart()
{
    ROS_INFO("TakeBall Start");
 
    cmd_.cmd_vel.linear.x = 0;
    cmd_.cmd_vel.linear.y = 0;
    cmd_.cmd_vel.angular.z = 0;

    cmd_.next_controller_state = TAKE_BALL;

    pub_stm32.publish(cmd_);

    ros::spinOnce();

    if(_is_ball_in_car == 1)
    {
        return BT::NodeStatus::SUCCESS;
    }
    else
    {
        return BT::NodeStatus::FAILURE;
    }
}
 
BT::NodeStatus r2_decision::TakeBall::onRunning()
{
    ROS_INFO("TakeBall Running");
    return BT::NodeStatus::RUNNING;

}

void r2_decision::TakeBall::onHalted()
{
    ROS_INFO("TakeBall Halted");
}
