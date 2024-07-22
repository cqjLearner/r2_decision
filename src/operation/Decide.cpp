#include "r2_decision/operation/Decide.h"

BT::NodeStatus r2_decision::Decide::tick()
{

    if(if_use_real_distance)
    {
        memcpy(DistanceCost,DistanceCost1,sizeof DistanceCost1);
    }

    for(i = 0; i < 5; i++)
    {
        if(if_use_plan2)
        {
            MyWinBasket[i] = 0;
            EnemyWinBasket[i] = 0;
            FullBasket[i] = 0;

            // nnn 情况
            if(BallSituation[i] == "null")
            {
                BallPower[i] = NoBall - DistanceCost[i];
            }

            // rrr、rbr、brr 情况(我方大胜框)
            else if(BallSituation[i] == "rrr" || BallSituation[i] == "rbr" || BallSituation[i] == "brr")
            {
                BallPower[i] = FullBall - DistanceCost[i];

                MyWinBasket[i] = 1;
                FullBasket[i] = 1;

                // _MyWinNum ++;
		        // _FullNum ++;
            }

            // bbb、brb、rbb 情况(敌方大胜框)
            else if(BallSituation[i] == "bbb" || BallSituation[i] == "brb" || BallSituation[i] == "rbb")
            {
                BallPower[i] = FullBall - DistanceCost[i];

                EnemyWinBasket[i] = 1;
                FullBasket[i] = 1;

                // _EnemyWinNum ++;
		        // _FullNum ++;
            }

            // rrb、bbr 情况
            else if(BallSituation[i] == "rrb" || BallSituation[i] == "bbr")
            {
                BallPower[i] = FullBall - DistanceCost[i];

                FullBasket[i] = 1;

		        // _FullNum ++;
            }

            // rrn 情况
            else if(BallSituation[i] == "rrn")
            {
                BallPower[i] = TwoR - DistanceCost[i];
                //我方大胜判断
                // if(_MyWinNum >= 2)
                if(SumFull(MyWinBasket) >= 2)
                {
                    BallPower[i] = BigWin - DistanceCost[i];
                }
            }

            // bbn 情况
            else if(BallSituation[i] == "bbn")
            {
                BallPower[i] = TwoB - DistanceCost[i];
                //敌方大胜判断
                // if(_EnemyWinNum >= 2)
                if(SumFull(EnemyWinBasket) >= 2)
                {
                    BallPower[i] = BigWin - DistanceCost[i];
                }
            }

            // brn、rbn 情况
            else if(BallSituation[i] == "brn" || BallSituation[i] == "rbn")
            {
                BallPower[i] = OneB_OneR - DistanceCost[i];
                //敌我双方大胜判断
                if(SumFull(MyWinBasket) >= 2 || SumFull(EnemyWinBasket) >= 2)
                {
                    BallPower[i] = BigWin - DistanceCost[i];
                }
            }

            // rnn 情况
            else if(BallSituation[i] == "rnn")
            {
                BallPower[i] = OneR - DistanceCost[i];
            }

            // bnn 情况
            else if(BallSituation[i] == "bnn")
            {
                BallPower[i] = OneB - DistanceCost[i];
            }
            
            else
            {
                ROS_INFO("BallSituation error!");
                return BT::NodeStatus::FAILURE;
            }
        }
        else
        {
            if(BallSituation[i] == "rrr" || BallSituation[i] == "rbr" || BallSituation[i] == "brr" ||
               BallSituation[i] == "bbb" || BallSituation[i] == "brb" || BallSituation[i] == "rbb" ||
               BallSituation[i] == "rrb" || BallSituation[i] == "bbr")
            {
                BallPower[i] = FullBall - DistanceCost[i];
                FullBasket[i] = 1;
            }
            else
            {
                BallPower[i] = -DistanceCost[i];
            }
        }
    }
    //寻找最大权值的框的下标
    _MaxPowerPos = std::max_element(BallPower,BallPower + 5) - BallPower;


    //////////////////////  测试  ///////////////////////
    ROS_INFO("The goal is the %d basket,power is %f",_MaxPowerPos + 1,BallPower[_MaxPowerPos]);
    for(int j = 0; j < 5; j++)
    {
        ROS_INFO("BallPower is %f",BallPower[j]);
    }
    ROS_INFO("_MyWinNum is %d",SumFull(MyWinBasket));
    ROS_INFO("_EnemyWinNum is %d",SumFull(EnemyWinBasket));
    ROS_INFO("_FullNum is %d",SumFull(FullBasket));
    //////////////////////  测试  ///////////////////////


    silo.silo_num = _MaxPowerPos + 1;
    if(SumFull(MyWinBasket) >= 3 || SumFull(EnemyWinBasket) >= 3 || SumFull(FullBasket) == 5)
    {
	    silo.competition_status = 0;
    }
    else
    {
	    silo.competition_status = 1;
    }
    pub_decide.publish(silo);

    return BT::NodeStatus::SUCCESS;
}
