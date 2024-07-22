#ifndef __MYPID_H
#define __MYPID_H

#include <ros/ros.h>
#include <boost/asio.hpp>

struct PID{
    float Kp;
    float Ki;
    float Kd;
    float error;
    float D_error;
    float integral;
    float OUT;
}base_PIDStructutr;

float pid_position_calculate(PID* PIDstu,float expect,float current);
void PidStructure_init(PID *PIDstu,float kp,float ki,float kd);

#endif
