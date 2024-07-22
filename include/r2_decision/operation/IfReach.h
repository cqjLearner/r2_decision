#pragma once
#include "ros/ros.h"
#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "r2_msgs/path_status.h"

namespace r2_decision
{
class IfReach : public BT::StatefulActionNode
{
public:
    IfReach(const std::string& name,const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name,config)
    {
        ros::NodeHandle nh;
        nh.getParam("if_reach_topic",if_reach_topic);

        //move_flag 标志小车是否移动到指定地点
        sub_flag = nh.subscribe(if_reach_topic,100,&IfReach::doFlag,this);

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
    ros::Subscriber sub_flag;

    std::string if_reach_topic;

    unsigned _ctrlFlag = 0; 

    void doFlag(const r2_msgs::path_status flag)
    {
        _ctrlFlag = flag.destination_arrived;
    }

};
}