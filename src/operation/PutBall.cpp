#include "r2_decision/operation/PutBall.h"

BT::NodeStatus r2_decision::PutBall::onStart()
{
    ROS_INFO("PutBall Start");
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus r2_decision::PutBall::onRunning()
{
    ROS_INFO("PutBall Running");

    if(_is_ball_in_car == 0)
    {
        ROS_INFO("Finish put the ball!");
        return BT::NodeStatus::SUCCESS;
    }
    else
    {
        // 停止吸球，滚筒放球 
        cmd_.next_controller_state = SHOOT_BALL;
        pub_stm32.publish(cmd_);
        
        ROS_INFO("Putting ball...");
        return BT::NodeStatus::RUNNING;
    }
}

void r2_decision::PutBall::onHalted()
{
    ROS_INFO("PutBall Halted");
}
