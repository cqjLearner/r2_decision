#include "r2_decision/RosTo32/mypid.h"

float pid_position_calculate(PID *PIDstu,float expect,float current){

    PIDstu -> D_error = (expect - current) - PIDstu->error;
    PIDstu -> error = expect - current;
    PIDstu -> integral += PIDstu ->error;

    PIDstu -> OUT = PIDstu->Kp * PIDstu->error + PIDstu->Ki *PIDstu->integral +PIDstu->Kd*PIDstu->D_error;
    return PIDstu-> OUT;
}

void PidStructure_init(PID *PIDstu,float kp,float ki,float kd){
    PIDstu ->Kp = kp;
    PIDstu ->Ki = ki;
    PIDstu ->Kd = kd;
    PIDstu ->D_error = 0;
    PIDstu ->error = 0;
    PIDstu ->integral = 0;
    PIDstu ->OUT = 0;

}