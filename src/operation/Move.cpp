#include "r2_decision/operation/Move.h"


BT::NodeStatus r2_decision::MoveBase::tick()
{
    //从黑板中获取键值对，即获取目标点  
    r2_msgs::path_cmd goal;
     if(!getInput<r2_msgs::path_cmd>("goal",goal))
    {
         // if I can't get this, there is something wrong with your BT. 
        // For this reason throw an exception instead of returning FAILURE 
        throw BT::RuntimeError("missing required input [goal]");
    }

    //将中止标志位设为否
    _aborted = false;
    
    //把收到的 goal 通过话题传递给路径规划函数 
    pub_goal.publish(goal);
    ROS_INFO("Sended goal:(%f %f %f)",goal.ball_position.x,goal.ball_position.y,goal.ball_position.z);  


    if (_aborted)
    {
        // 这只有在 halt() 被调用时才会发生，即中止时才会发生
        ROS_ERROR("MoveBase aborted");
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::SUCCESS;

}