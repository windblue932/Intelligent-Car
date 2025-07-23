/*
 * pid.h
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */

#ifndef CODE_PIDPID_H_
#define CODE_PIDPID_H_

#include "zf_common_headfile.h"



typedef struct PID
{
        float Kp;
        float Ki;
        float Kd;
        float LowPass;

        float Out_P;
        float Out_I;
        float Out_D;

        float PrevError;
        float LastError;
        float Error;
        float Lastdata;
}PID;


typedef struct pid_param_tt{
    float kp;         //P
    float ki;         //I
    float kd;         //D
    float imax;       //积分限幅

    float out_p;  //KP输出
    float out_i;  //KI输出
    float out_d;  //KD输出
    float out;    //pid输出

    float integrator; //< 积分值
    float last_error; //< 上次误差
    float last_derivative; //< 上次误差与上上次误差之差
    unsigned long last_t;     //< 上次时间
} pid_param_tt;


#define PID_CREATE(_kp,_ki,_kd,_low_pass)\
{                                       \
    .Kp=_kp,                            \
    .Ki=_ki,                            \
    .Kd=_kd,                            \
    .LowPass=_low_pass,                 \
    .Out_P=0,                           \
    .Out_I=0,                           \
    .Out_D=0,                           \
}


#define PID_CREATETEST(_kp,_ki,_kd)\
{                                       \
    .kp=_kp,                            \
    .ki=_ki,                            \
    .kd=_kd,                            \
    .out_p=0,                           \
    .out_i=0,                           \
    .out_d=0,                           \
}




extern float prev_error;
extern float integral;

float PID_Increase(PID*PID ,float now_data ,float target_data);
float PidIncCtrltest(volatile pid_param_tt * pid, int16_t error);
float PidLocCtrltest(volatile pid_param_tt * pid, float error);
int fuzzify_e(float x);
int fuzzify_de(float x);
float fuzzy_pid(volatile pid_param_tt * pid, float error);

#endif /* CODE_PIDPID_H_ */
