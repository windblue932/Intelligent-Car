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
extern uint8_t send_data_flag;      //��־λ����

extern volatile uint16_t CurrentServoDty;

extern PID motor_dir;
extern PID motor_pid_l;//���
extern PID motor_pid_r;//���

extern pid_param_tt motor_pid_turn;
extern pid_param_tt motor_pid_mutu;
extern pid_param_tt motor_pid_dist;

extern pid_param_tt Servo_dire_pid;
extern pid_param_tt Servo_turn_pid;
extern pid_param_tt Servo_mutu_pid;

extern float KLI_DEC;
extern float KLO_DEC;
extern float KRI_DEC;
extern float KRO_DEC;

extern float target_dist;
extern float base_speed;
extern float k_dec;

extern uint16_t isr_count;
extern bool flag;
extern uint32_t time0;
extern uint32_t time1;
extern uint32_t time2;

extern volatile float param_1[11][11];             //Ԥ������ײ���
extern volatile float param_2[11][11];
/************************************PID�ṹ��***********************************************/
typedef struct
{
    volatile bool Motor_Creazy;                 //����ַ���
    volatile bool Motor_Stem;                   //���������
    volatile bool GOGOGO;                       //������־λ

}flag_t;

extern volatile flag_t Flag;


#endif /* VARIABLES_H_ */
