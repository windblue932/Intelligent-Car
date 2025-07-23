/*
 * variables.h
 *
 *  Created on: 2023��5��11��
 *      Author: ZGX
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_
#include "zf_common_headfile.h"
#include "stdint.h"
#include "stdbool.h"
/************************************������***********************************************/
#define pid_out_limit 5000
/************************************�����������***********************************************/
//������pwm������
extern int pid_down_left_limit;  //��������
extern int pid_up_left_limit;    //��������
extern int pid_down_right_limit; //��������
extern int pid_up_right_limit;   //��������

extern struct PID motor_pid_d;
extern struct PID motor_pid_l;
extern struct PID motor_pid_r;

extern struct pid_param_tt servo_pid_dire;
extern struct pid_param_tt servo_pid_turn;
extern struct pid_param_tt servo_pid_muta;

extern float Kp_base;
extern float Ki_base;
extern float Kd_base;

extern int  AIM_LINE_STRAIGHT;
extern int  AIM_LINE_TURN;
extern int  AIM_LINE_MUTATION;

extern int  SPEED_MUTATION;
extern int  SPEED_TURN;
extern int SPEED_STRAIGHT;

extern float fuzzy_Kp[7][7];
extern float fuzzy_Ki[7][7];
extern float fuzzy_Kd[7][7];

extern volatile int Purpost_Speed;           //Ŀ���ٶ�
extern volatile float KLI_DEC;
extern volatile float KLO_DEC;
extern volatile float KRI_DEC;
extern volatile float KRO_DEC;

extern volatile float LOCPID_THRESHOLD;

extern int creazy_count;
extern int stem_count;

extern volatile float param_1[10][10];             //Ԥ������ײ���
extern volatile float param_2[10][10];
typedef struct
{
    volatile bool Left_Ring_Find;               //Բ�����ڲ��߱�־λ
    volatile bool Left_Ring_Turn;               //��Բ�����߱�־λ
    volatile bool Left_Ring_Out;                //Բ���ڱ�־λ
    volatile bool Left_Ring_Out_Mend1;          //�����̶���Ǳ�־λ
    volatile bool Left_Ring_Out_Mend2;          //�������߱�־λ
    volatile bool Left_Ring_OnlyOnce;           //Բ��ֻ��һ��
    volatile bool Left_Ring;                    //Բ��ȫ�� ������������Ԫ�ش���

    volatile bool Right_Ring_Find;
    volatile bool Right_Ring_Turn;
    volatile bool Right_Ring_Out;
    volatile bool Right_Ring_Out_Mend1;
    volatile bool Right_Ring_Out_Mend2;
    volatile bool Right_Ring_OnlyOnce;
    volatile bool Right_Ring;

    //ʮ��
    volatile bool CrossRoad_Find;               //ʮ��ʶ���־λ
    volatile bool CrossRoad_Block;              //ʮ�����α�־λ
    volatile bool crossRight;                   //ʮ���Ҳ���

    volatile bool crossLeft;                    //ʮ���󲿷�
    volatile bool CrossIn;                      //����ʮ�ֱ�־λ
    volatile bool CrossOut;                      //����ʮ�ֱ�־λ

    volatile bool Garage_Find;
    volatile bool Park;
    volatile bool Fan;                          //��ѹ���ȱ�־λ
    volatile bool GOGOGO;                       //������־λ
    volatile bool Motor_Creazy;                 //�����ת��־λ
    volatile bool Motor_Stem;                   //�����ת��־λ

}flag_t;

extern volatile flag_t Flag;
/**************************************Բ������***********************************************/
/**************************************�������***********************************************/
typedef struct
{
    volatile float Servo_filter;
    volatile float cam_servo_temp_1;
}servo_param_t;
extern volatile servo_param_t servo_param;

/**************************************�������***********************************************/

/**************************************���ƾ������***********************************************/
extern volatile uint16_t isr_count;

extern bool flag;
extern int time0;
extern int time1;
extern int time2;
extern int time3;
extern int time4;
extern int time5;
extern int time6;
extern int time7;
#endif /* VARIABLES_H_ */
