/*
 * pid.h
 *
 *  Created on: 2025��3��10��
 *      Author: 15208
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

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
    float imax;       //�����޷�

    float out_p;  //KP���
    float out_i;  //KI���
    float out_d;  //KD���
    float out;    //pid���

    float integrator; //< ����ֵ
    float last_error; //< �ϴ����
    float last_derivative; //< �ϴ���������ϴ����֮��
    unsigned long last_t;     //< �ϴ�ʱ��
} pid_param_tt;


extern struct PID motor_pid_l;
extern struct PID motor_pid_r;

extern struct pid_param_tt motor_pid_left;
extern struct pid_param_tt motor_pid_right;
extern struct pid_param_tt motor_pid_direction;
extern struct pid_param_tt motor_pid_back;
extern struct pid_param_tt Servo_Cam_pid;

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


#define pid_out_limit 9000

float PidLocCtrl(volatile pid_param_tt * pid, float error);
float PIDIncCtrl(PID*PID ,float now_data ,float target_data);
float PidIncCtrltest(volatile pid_param_tt * pid, int16_t error);
float PidLocCtrltest(volatile pid_param_tt * pid, float error);
#endif /* CODE_PID_H_ */
