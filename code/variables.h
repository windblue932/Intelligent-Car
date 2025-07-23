/*
 * variables.h
 *
 *  Created on: 2023年5月11日
 *      Author: ZGX
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "zf_common_headfile.h"
#include "stdint.h"
#include "stdbool.h"
/************************************处理结果***********************************************/
extern uint8_t send_data_flag;      //标志位发送

extern volatile uint16_t CurrentServoDty;

extern PID motor_dir;
extern PID motor_pid_l;//电机
extern PID motor_pid_r;//电机

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

extern volatile float param_1[11][11];             //预设的两套参数
extern volatile float param_2[11][11];
/************************************PID结构体***********************************************/
typedef struct
{
    volatile bool Motor_Creazy;                 //电机又疯了
    volatile bool Motor_Stem;                   //电机被堵了
    volatile bool GOGOGO;                       //发车标志位

}flag_t;

extern volatile flag_t Flag;


#endif /* VARIABLES_H_ */
