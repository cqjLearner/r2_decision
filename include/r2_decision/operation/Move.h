#pragma once
#include "ros/ros.h"
#include "std_msgs/Int16.h"
#include "tf/transform_datatypes.h"
#include "r2_decision/bt_action_node.h"
#include "r2_decision/bt_service_node.h"
#include "r2_msgs/path_cmd.h"


enum
{
    FW_INVERTED=2,  //摩擦轮反转
    FW_TAKE_BALL,   //摩擦轮取球
    FW_CONTROLLER_OFF,
    BP_SHOOT_BALL,  //滚筒出球
    BP_ABANDON_BALL,//滚筒筛球
	  BP_INVERTED,    //滚筒反转
    BP_CONTROLLER_OFF,  //机构关闭

    LINEAR_ACTUATOR_GO,
    LINEAR_ACTUATOR_BACK,
    LINEAR_ACTUATOR_OFF
};

enum
{
    TAKE_BALL=2,
    FILTER_BALL,
    SHOOT_BALL,
    CONTROLLER_OFF,
    CONTROLLER_ERROR
};

   
namespace BT
{
template <>
inline r2_msgs::path_cmd convertFromString(StringView key)
{
  // 把输入的 key 按分号分隔
  auto parts = BT::splitString(key, ';');
  if (parts.size() != 5)
  {
    throw BT::RuntimeError("invalid input)");
  }
  else
  {
    r2_msgs::path_cmd output; 
    output.ball_position.x = convertFromString<double>(parts[0]);
    output.ball_position.y = convertFromString<double>(parts[1]);
    output.ball_position.z = convertFromString<double>(parts[2]);
    output.PathMode = convertFromString<double>(parts[3]);
    output.take_or_put = convertFromString<double>(parts[4]);
    return output;
  }
}

}  // namespace BT 
namespace r2_decision
{
//异步行为节点 
class MoveBase : public BT::AsyncActionNode
{
public:
  MoveBase(const std::string& name, const BT::NodeConfiguration& config)
    : BT::AsyncActionNode(name, config)
  {

    ros::NodeHandle nh;
    nh.getParam("publish_move_goal_topic",publish_move_goal_topic);

    //发送目标给路径规划
    pub_goal = nh.advertise<r2_msgs::path_cmd>(publish_move_goal_topic,100);

  }

  // It is mandatory to define this static method.
  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<r2_msgs::path_cmd>("goal") };
  }

  virtual BT::NodeStatus tick() override;

  
  virtual void halt() override
  {
    _aborted = true;
  }
  

private:

  ros::Publisher pub_goal;

  bool _aborted;

  std::string publish_move_goal_topic;

  
};
}  // namespace r2_decision

