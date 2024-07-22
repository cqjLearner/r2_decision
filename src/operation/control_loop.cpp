#include "r2_decision/operation/control_loop.h"

#ifdef SUPPORT_OPENVINO
#include "openvino_event.h"
#endif

int main(int argc, char *argv[])
{
    ros::init(argc,argv,"control_loop");
    ros::NodeHandle nh;

    std::string tree_config_path;
    nh.getParam("/tree_config_path",tree_config_path);

    BT::BehaviorTreeFactory factory;
    BT::NodeStatus status = BT::NodeStatus::IDLE;

    factory.registerNodeType<r2_decision::MoveBase>("MoveBase");
    factory.registerNodeType<r2_decision::FilterBall>("FilterBall");
    factory.registerNodeType<r2_decision::TakeBall>("TakeBall");
    factory.registerNodeType<r2_decision::PutBall>("PutBall");
    factory.registerNodeType<r2_decision::BlackBoardRead>("BlackBoardRead");
    factory.registerNodeType<r2_decision::Decide>("Decide");
    factory.registerNodeType<r2_decision::IfReach>("IfReach");
    
    auto tree = factory.createTreeFromFile(tree_config_path);
    // Create a logger
    BT::StdCoutLogger logger_cout(tree);  
#ifdef ZMQ_FOUND
// This logger publish status changes using ZeroMQ. Used by Groot
    PublisherZMQ publisher_zmq(tree);
#endif
    ros::Rate rate(10);
    while(ros::ok())
    {
        status = tree.tickRoot();
        std::cout << status << std::endl;
        rate.sleep();
        ros::spinOnce();
    }
    return 0;
}
