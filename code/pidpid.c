/*
 * pid.c
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */

#include "zf_common_headfile.h"

//位置式PID
float PidLocCtrl(volatile pid_param_tt * pid, float error) {

    pid->integrator += error;

    pid->integrator = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    return pid->out;
}

//增量式PID
float PIDIncCtrl(PID*PID ,float now_data ,float target_data){
    PID->Error=target_data-now_data;
    PID->Out_P=(PID->Error - PID->LastError);
    PID->Out_I=PID->Error;
    PID->Out_D=(PID->Error -2*PID->LastError + PID->PrevError);
    PID->PrevError=0.9 * PID->LastError + 0.1 * PID->PrevError;
    PID->LastError=0.9 *PID->Error + 0.1 * PID->LastError;
    PID->Lastdata = now_data;

    return (PID->Kp*PID->Out_P+PID->Ki*PID->Out_I+PID->Kd*PID->Out_D);
}


