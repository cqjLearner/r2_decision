#pragma once
#include "ros/ros.h"
#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "r2_decision/operation/Move.h"
#include "r2_msgs/stm32.h"
#include "r2_msgs/controller_cmd.h"


namespace r2_decision
{
class PutBall : public BT::StatefulActionNode
{
public:
    PutBall(const std::string& name,const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name,config)
    {
        ros::NodeHandle nh;

        nh.getParam("receive_stm32_topic",receive_stm32_topic);
        nh.getParam("publish_stm32_topic",publish_stm32_topic);

        sub_stm32 = nh.subscribe(receive_stm32_topic,100,&PutBall::doSTM32,this);
        pub_stm32 = nh.advertise<r2_msgs::controller_cmd>(publish_stm32_topic,1);

        cmd_.cmd_vel.linear.x = 0;
        cmd_.cmd_vel.linear.y = 0;
        cmd_.cmd_vel.angular.z = 0;
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

    static BT::PortsList providedPorts()
    {
        BT::PortsList ports_list;
        ports_list.insert(BT::InputPort<int>("goalB")); 
        return ports_list;
    }


private:
    ros::Subscriber sub_stm32;
    ros::Publisher pub_stm32;

    std::string receive_stm32_topic;
    std::string publish_stm32_topic;

    uint8_t _is_ball_in_car = 1;

    //发给32控制器，控制筛球
    r2_msgs::controller_cmd cmd_;

    void doSTM32(const r2_msgs::stm32 flag)
    {
        // flag.is_ball_in_car：小车内没球时为0，有球且为正确球为1
        _is_ball_in_car = flag.ball_state;
    }
};
}