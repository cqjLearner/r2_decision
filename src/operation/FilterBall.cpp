#include "r2_decision/operation/FilterBall.h"

//每次 tick 会进入 onStart，其返回 RUNNING 时进入 onRunning
BT::NodeStatus r2_decision::FilterBall::onStart()
{

    if(_if_no_need_filter == 1)
    {
        ROS_INFO("It's no need to filter ball.");
        // 滚筒摩擦轮都停
        cmd_.next_controller_state = FW_CONTROLLER_OFF;
        cmd_.next_controller_state = BP_CONTROLLER_OFF;
        pub_stm32.publish(cmd_);
        return BT::NodeStatus::SUCCESS;
    }
    
    return BT::NodeStatus::RUNNING;
}

// 会进入 onRunning 说明需要筛球，故发送筛球标志位 
// 同时返回 FAILURE 让动作从重新识球开始
BT::NodeStatus r2_decision::FilterBall::onRunning()
{
    ROS_INFO("_if_no_need_filter is :%d",_if_no_need_filter);
    
    // 停止吸球，滚筒筛球
    cmd_.next_controller_state = FW_CONTROLLER_OFF;
    cmd_.next_controller_state = BP_ABANDON_BALL;
    pub_stm32.publish(cmd_);

    ROS_INFO("Filtering ball...");
    return BT::NodeStatus::FAILURE;

}

void r2_decision::FilterBall::onHalted()
{
    ROS_INFO("FilterBall Halted");
}
