#pragma once
#include "r2_decision/bt_action_node.h"
#include "r2_decision/bt_service_node.h"
#include "std_msgs/Float64MultiArray.h"
#include "r2_decision/operation/Move.h"
#include "r2_msgs/controller_cmd.h"


namespace r2_decision
{
//同步行为节点  
class BlackBoardRead : public BT::SyncActionNode
{
public:
    //构造函数 
    BlackBoardRead(const std::string& name,const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name,config)
    {
        ros::NodeHandle nh;
        nh.getParam("camera_catch_ball_topic",camera_catch_ball_topic);
        nh.getParam("publish_stm32_topic",publish_stm32_topic);

        goal_sub = nh.subscribe(camera_catch_ball_topic,100,&BlackBoardRead::doGoal,this);
        pub_stm32 = nh.advertise<r2_msgs::controller_cmd>(publish_stm32_topic,1);

    }

    //设置黑板端口
    static BT::PortsList providedPorts()
    {
        BT::PortsList ports_list;
        ports_list.insert(BT::OutputPort<r2_msgs::path_cmd>("goal"));
        return ports_list;
    }

    //黑板端口赋值
    BT::NodeStatus tick() override
    {

        setOutput("goal",goal_);
        ROS_INFO("Sending goal:(%f %f %f)",goal_.ball_position.x,
                                           goal_.ball_position.y,
                                           goal_.ball_position.z);
        
        cmd_.next_controller_state = FILTER_BALL;
        pub_stm32.publish(cmd_);
        
        if(goal_.ball_position.x != 0 && goal_.ball_position.y != 0) return BT::NodeStatus::SUCCESS;
        else return BT::NodeStatus::FAILURE;
    }

private:
    ros::Subscriber goal_sub;
    ros::Publisher pub_stm32;

    std::string camera_catch_ball_topic;
    std::string publish_stm32_topic;

    r2_msgs::path_cmd goal_;
    r2_msgs::controller_cmd cmd_;



    void doGoal(const std_msgs::Float64MultiArray msg)
    {
        goal_.ball_position.x = msg.data[0];
        goal_.ball_position.y = msg.data[1];
        goal_.ball_position.z = atan(goal_.ball_position.x / goal_.ball_position.y);
        //取球模式
        goal_.PathMode = 4;
        goal_.take_or_put = 5;
    }

};
}// namespace r2_decision