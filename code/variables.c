    /*
 * variables.c
 *
 *  Created on: 2023年5月11日
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
/************************************处理结果******************************************/

uint8_t send_data_flag = 0;//标志位发送

volatile uint16_t CurrentServoDty = Servo_Center_Mid;          //舵机设置占空比


PID motor_dir  = PID_CREATE(1,0,0,1);
PID motor_pid_l = PID_CREATE(35,25,0,1); //左电机  35,25,0,1
PID motor_pid_r = PID_CREATE(35,25,0,1); //右电机

pid_param_tt motor_pid_turn = PID_CREATETEST(0,0,0);//小转弯差速0.5,0,3
pid_param_tt motor_pid_mutu = PID_CREATETEST(2.2,0,5);//大转弯差速

pid_param_tt motor_pid_dist = PID_CREATETEST(3.5,0.5,7); //距离控制   3,0,5   8 0 16

pid_param_tt Servo_dire_pid = PID_CREATETEST(3,0,8);//6.5,0,90   5.5,0,90   2 0 6
pid_param_tt Servo_turn_pid = PID_CREATETEST(3,0,8);//  4.5,0,70   4,0,20
pid_param_tt Servo_mutu_pid = PID_CREATETEST(3,0,8);

float KLI_DEC = 0.1;
float KLO_DEC = 0;
float KRI_DEC = 0.1;   //内轮减速
float KRO_DEC = 0;   //外轮加速

float target_dist = 7;
float base_speed = 195; //185   195
float k_dec = 1;

uint16_t isr_count = 0;
bool flag = 0;
uint32_t time0 = 0;
uint32_t time1 = 0;
uint32_t time2 = 0;

volatile float param_1[11][11] = {0};            //预设的两套参数
volatile float param_2[11][11] = {0};
/************************************PID结构体***********************************************/
volatile flag_t Flag=
{
        .Motor_Creazy = false,                      //电机疯了
        .Motor_Stem = false,                        //电机被堵
        .GOGOGO = false,                            //发车标志位
};

