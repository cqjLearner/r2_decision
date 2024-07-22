#include "r2_decision/operation/IfReach.h"

BT::NodeStatus r2_decision::IfReach::onStart()
{
    if(_ctrlFlag == 1)
    {
        ROS_INFO("Reach the goal");
        return BT::NodeStatus::SUCCESS;
    }
    return BT::NodeStatus::FAILURE;
}
 
BT::NodeStatus r2_decision::IfReach::onRunning()
{

    return BT::NodeStatus::RUNNING;

}

void r2_decision::IfReach::onHalted()
{
    ROS_INFO("TakeBall Halted");
}