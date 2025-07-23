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
#define pid_out_limit 5000
/************************************电机驱动参数***********************************************/
//左右轮pwm上下限
extern int pid_down_left_limit;  //左轮下限
extern int pid_up_left_limit;    //左轮上限
extern int pid_down_right_limit; //右轮下限
extern int pid_up_right_limit;   //右轮上限

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

extern volatile int Purpost_Speed;           //目标速度
extern volatile float KLI_DEC;
extern volatile float KLO_DEC;
extern volatile float KRI_DEC;
extern volatile float KRO_DEC;

extern volatile float LOCPID_THRESHOLD;

extern int creazy_count;
extern int stem_count;

extern volatile float param_1[10][10];             //预设的两套参数
extern volatile float param_2[10][10];
typedef struct
{
    volatile bool Left_Ring_Find;               //圆环出口补线标志位
    volatile bool Left_Ring_Turn;               //进圆环补线标志位
    volatile bool Left_Ring_Out;                //圆环内标志位
    volatile bool Left_Ring_Out_Mend1;          //出环固定打角标志位
    volatile bool Left_Ring_Out_Mend2;          //出环补线标志位
    volatile bool Left_Ring_OnlyOnce;           //圆环只跑一次
    volatile bool Left_Ring;                    //圆环全程 用于屏蔽其他元素处理

    volatile bool Right_Ring_Find;
    volatile bool Right_Ring_Turn;
    volatile bool Right_Ring_Out;
    volatile bool Right_Ring_Out_Mend1;
    volatile bool Right_Ring_Out_Mend2;
    volatile bool Right_Ring_OnlyOnce;
    volatile bool Right_Ring;

    //十字
    volatile bool CrossRoad_Find;               //十字识别标志位
    volatile bool CrossRoad_Block;              //十字屏蔽标志位
    volatile bool crossRight;                   //十字右部分

    volatile bool crossLeft;                    //十字左部分
    volatile bool CrossIn;                      //进入十字标志位
    volatile bool CrossOut;                      //进入十字标志位

    volatile bool Garage_Find;
    volatile bool Park;
    volatile bool Fan;                          //负压风扇标志位
    volatile bool GOGOGO;                       //发车标志位
    volatile bool Motor_Creazy;                 //电机疯转标志位
    volatile bool Motor_Stem;                   //电机堵转标志位

}flag_t;

extern volatile flag_t Flag;
/**************************************圆环参数***********************************************/
/**************************************舵机参数***********************************************/
typedef struct
{
    volatile float Servo_filter;
    volatile float cam_servo_temp_1;
}servo_param_t;
extern volatile servo_param_t servo_param;

/**************************************电机参数***********************************************/

/**************************************控制距离参数***********************************************/
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
