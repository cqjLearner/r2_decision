#pragma once
#include "r2_decision/bt_action_node.h"
#include "r2_decision/bt_service_node.h"
// #include "std_msgs/Float64MultiArray.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/String.h"
#include <algorithm>
#include <numeric>
#include "math.h"
#include "r2_decision/operation/Move.h"
#include "yolov5_ros_msgs/Silos.h"
#include "r2_msgs/decision.h"

struct Pose2D
{
    float x;
    float y;
    float theta;
};

//各种情况的权值，红色为我方球，蓝色为敌方球
enum
{
    OneB = 10,
    OneR = 20,
    NoBall = 30,
    TwoB = 40,
    TwoR = 50,
    OneB_OneR = 60,
    FullBall = -999,
    BigWin = 999
};
 
namespace r2_decision
{
//同步行为节点 
class Decide : public BT::SyncActionNode
{
public:
    Decide(const std::string& name ,const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name,config)
    {
        ros::NodeHandle nh;
        nh.getParam("camera_decide_topic",camera_decide_topic);
        nh.getParam("laser_topic",laser_topic);
	    nh.getParam("publish_decide_topic",publish_decide_topic);

        nh.getParam("path_plan/silo_coordinate/silo1/x",silo1_x);  nh.getParam("path_plan/silo_coordinate/silo1/y",silo1_y);
        nh.getParam("path_plan/silo_coordinate/silo2/x",silo2_x);  nh.getParam("path_plan/silo_coordinate/silo2/y",silo2_y);
        nh.getParam("path_plan/silo_coordinate/silo3/x",silo3_x);  nh.getParam("path_plan/silo_coordinate/silo3/y",silo3_y);
        nh.getParam("path_plan/silo_coordinate/silo4/x",silo4_x);  nh.getParam("path_plan/silo_coordinate/silo4/y",silo4_y);
        nh.getParam("path_plan/silo_coordinate/silo5/x",silo5_x);  nh.getParam("path_plan/silo_coordinate/silo5/y",silo5_y);
        
        nh.getParam("/if_use_plan2",if_use_plan2);
        nh.getParam("/if_use_real_distance",if_use_real_distance);

        sub_camera = nh.subscribe(camera_decide_topic,100,&Decide::doCamera,this);
        sub_laser = nh.subscribe(laser_topic,100,&Decide::doLaser,this);
        pub_decide = nh.advertise<r2_msgs::decision>(publish_decide_topic,1);

        BasketPos[0].x = silo1_x;     BasketPos[0].y = silo1_y;
        BasketPos[1].x = silo2_x;     BasketPos[1].y = silo2_y;
        BasketPos[2].x = silo3_x;     BasketPos[2].y = silo3_y;
        BasketPos[3].x = silo4_x;     BasketPos[3].y = silo4_y;
        BasketPos[4].x = silo5_x;     BasketPos[4].y = silo5_y;

    }

    static BT::PortsList providedPorts()
    {
        BT::PortsList ports_list;
        ports_list.insert(BT::OutputPort<r2_msgs::path_cmd>("goal"));
        return ports_list;
    }

    
    virtual BT::NodeStatus tick() override;

    int SumFull(int Basket[])
    {
        int res = 0;
        for(int k = 0; k < 5; k++)
        {
            res += Basket[k];
        }
        return res;
    }


private:

    ros::Subscriber sub_camera;
    ros::Subscriber sub_laser;
    ros::Publisher pub_decide;

    std::string camera_decide_topic;
    std::string laser_topic;
    std::string publish_decide_topic;

    r2_msgs::decision silo;

    Pose2D BasketPos[5];
    float laser_X;
    float laser_Y;

    float silo1_x,silo1_y;
    float silo2_x,silo2_y;
    float silo3_x,silo3_y;
    float silo4_x,silo4_y;
    float silo5_x,silo5_y;

    bool if_use_plan2;
    bool if_use_real_distance;

    // std::string BeforeBallSituation[5];   //前一次框中球的情况
    std::string BallSituation[5];         //五个框中球的情况

    float DistanceCost[5] = {3,2,1,2,3};         //五个框由近到远的代价

    float DistanceCost1[5] = {0};

    float BallPower[5] = {-1,-2,-3,-4,-5};       //五个框的初始权值

    int _MyWinNum = 0;      //我方大胜框个数
    int _EnemyWinNum = 0;   //敌方大胜框个数
    int _FullNum = 0;		//满球框数

    // 记录敌我双方大胜框和满框的位置
    int MyWinBasket[5] = {0};
    int EnemyWinBasket[5] = {0};
    int FullBasket[5] = {0};


    int _MaxPowerPos = 0;   //最大权值的框的位置

    int i = 0;


    //用于处理相机传来数据的回调
    void doCamera(const yolov5_ros_msgs::Silos msg)
    {
        BallSituation[0] = msg.a;
        BallSituation[1] = msg.b;
        BallSituation[2] = msg.c;
        BallSituation[3] = msg.d;
        BallSituation[4] = msg.e;
    }

    //获取雷达的坐标数据(以雷达为坐标系)，让距离代价动态变化
    void doLaser(const nav_msgs::Odometry msg)
    {
        float DistanceSum = 0;
        laser_X = msg.pose.pose.position.x;
        laser_Y = msg.pose.pose.position.y;

        // 计算当前位置到五个框的欧氏距离
        for(int i = 0; i < 5; i++)
        {
            DistanceCost1[i] = sqrt((laser_X - BasketPos[i].x)*(laser_X - BasketPos[i].x)
                                    + (laser_Y - BasketPos[i].y)*(laser_Y - BasketPos[i].y));
            DistanceSum += DistanceCost1[i];
        }

        // 归一化
        for(int i = 0; i < 5; i++)
        {
            DistanceCost1[i] = DistanceCost1[i] * 10 / DistanceSum;
        }
    }

};
}// namespace r2_decision