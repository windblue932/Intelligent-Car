/*
 * pid.c
 *
 *  Created on: 2025��3��10��
 *      Author: 15208
 */

#include "zf_common_headfile.h"

//����ʽPID
float PID_Increase(PID*PID ,float now_data ,float target_data)
{
    PID->Error=target_data-now_data;
    PID->Out_P=(PID->Error - PID->LastError);
    PID->Out_I=PID->Error;
    PID->Out_D=(PID->Error -2*PID->LastError + PID->PrevError);
    //PID->PrevError = 0.9 * PID->LastError + 0.1 * PID->PrevError;
    //PID->LastError = 0.9 * PID->Error + 0.1 * PID->LastError;
    PID->PrevError = PID->LastError;
    PID->LastError = PID->Error;
    PID->Lastdata = now_data;

    return (PID->Kp*PID->Out_P+PID->Ki*PID->Out_I+PID->Kd*PID->Out_D);
}



float PidIncCtrltest(volatile pid_param_tt * pid, int16_t error) {

    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error;
    pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);

    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;

    pid->out += pid->out_p + pid->out_i + pid->out_d;

    pid->out = LIMIT(pid->out, -10000, 10000);

    return pid->out;
}


float PidLocCtrltest(volatile pid_param_tt * pid, float error) {
    /* �ۻ���� */
    pid->integrator += error;

    /* ����޷� */
    pid->integrator = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    return pid->out;
}

// ģ��������������e��deӳ�䵽[-1, 0, 1]������С������
int fuzzify_e(float x) {
    if (x <= -35) return -3;
    else if (x <= -25) return -2;
    else if (x <= -10) return -1;
    else if (x < 10) return 0;
    else if (x < 25) return 1;
    else if (x < 35) return 2;
    else return 3;
}


int fuzzify_de(float x) {
    // ���� d_error [-10, 10]
    if (x <= -7) return -3;
    else if (x <= -4) return -2;
    else if (x <= -2) return -1;
    else if (x < 2) return 0;
    else if (x < 4) return 1;
    else if (x < 7) return 2;
    else return 3;
}


// PID״̬����
float prev_error = 0.0;
float integral = 0.0;

//ģ��PID
float fuzzy_pid(volatile pid_param_tt * pid, float error) {
    float de = (error - prev_error);
    prev_error = error;

    // ģ������������仯��
    int e_level  = fuzzify_e(error);
    int de_level = fuzzify_de(de);

    // ȡ����������ƫ��+1�����[0,1,2]��
    float delta_Kp = fuzzy_Kp[e_level + 3][de_level + 3];
    float delta_Ki = fuzzy_Ki[e_level + 3][de_level + 3];
    float delta_Kd = fuzzy_Kd[e_level + 3][de_level + 3];

    // ��̬����PID����
    pid->kp = Kp_base + delta_Kp;
    pid->ki = Ki_base + delta_Ki;
    pid->kd = Kd_base + delta_Kd;

    // PID����
    integral += error;
    float derivative = de;

    float u = pid->kp * error + pid->ki * integral + pid->kd * derivative;
    return u;
}




