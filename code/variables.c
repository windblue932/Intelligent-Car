    /*
 * variables.c
 *
 *  Created on: 2023年5月11日
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
/************************************处理结果******************************************/
int pid_down_left_limit = 8000;  //左轮下限
int pid_up_left_limit = 5000;    //左轮上限
int pid_down_right_limit = 5500; //右轮下限
int pid_up_right_limit = 5000;   //右轮上限
/************************************视觉处理参数******************************************/
int AIM_LINE_STRAIGHT = 10;
int AIM_LINE_TURN = 20;
int AIM_LINE_MUTATION = 20;

/************************************电机驱动参数***********************************************/
PID motor_pid_d = PID_CREATE(1, 0, 0, 1);    //直电机
PID motor_pid_l = PID_CREATE(40, 20, 0, 1);    //左电机    （25 10 5 8）  (45, 11, 5, 1)  45, 13, 5, 1  40, 18, 0, 1    45, 28, 0, 1
PID motor_pid_r = PID_CREATE(40, 20, 0, 1);    //右电机

pid_param_tt servo_pid_dire=PID_CREATETEST(2, 0, 5);     //直道舵机
pid_param_tt servo_pid_turn=PID_CREATETEST(3.5, 0, 7);        //小弯舵机    //7.4下午13.00：6 0 15打算上调
pid_param_tt servo_pid_muta=PID_CREATETEST(3.5, 0, 7);        //大弯舵机

//初始参数
float Kp_base = 2.0;
float Ki_base = 0.0;
float Kd_base = 7.0;

int SPEED_MUTATION = 170;
int SPEED_TURN = 170;
int SPEED_STRAIGHT = 180;

volatile int Purpost_Speed = 180;

int creazy_count = 0;
int stem_count = 0;

volatile float KLI_DEC = 1;         //1.45    0.15
volatile float KLO_DEC = 0;
volatile float KRI_DEC = 1;
volatile float KRO_DEC = 0;
volatile float LOCPID_THRESHOLD = 25;
volatile uint16_t isr_count = 0;

volatile float param_1[10][10] = {0};            //预设的两套参数
volatile float param_2[10][10] = {0};

//ΔKp模糊增量表
float fuzzy_Kp[7][7] = {
    { 0.2,  0.2,  0.1,  0.1,  0.0,  0.0,  0.0 },
    { 0.2,  0.1,  0.1,  0.1,  0.0,  0.0, -0.1 },
    { 0.1,  0.1,  0.1,  0.0,  0.0, -0.1, -0.1 },
    { 0.1,  0.1,  0.0,  0.0,  0.0, -0.1, -0.1 },
    { 0.1,  0.0,  0.0,  0.0, -0.1, -0.1, -0.1 },
    { 0.0,  0.0, -0.1, -0.1, -0.1, -0.2, -0.2 },
    { 0.0, -0.1, -0.1, -0.1, -0.2, -0.2, -0.2 }
};

//ΔKi模糊增量表
float fuzzy_Ki[7][7] = {
    { 0.02,  0.02,  0.01,  0.01,  0.00,  0.00,  0.00 },
    { 0.02,  0.01,  0.01,  0.01,  0.00,  0.00, -0.01 },
    { 0.01,  0.01,  0.01,  0.00,  0.00, -0.01, -0.01 },
    { 0.01,  0.01,  0.00,  0.00,  0.00, -0.01, -0.01 },
    { 0.01,  0.00,  0.00,  0.00, -0.01, -0.01, -0.01 },
    { 0.00,  0.00, -0.01, -0.01, -0.01, -0.02, -0.02 },
    { 0.00, -0.01, -0.01, -0.01, -0.02, -0.02, -0.02 }
};

//ΔKd模糊增量表
float fuzzy_Kd[7][7] = {
    { 0.05,  0.04,  0.03,  0.02,  0.01,  0.00,  0.00 },
    { 0.04,  0.03,  0.02,  0.02,  0.01,  0.00, -0.01 },
    { 0.03,  0.02,  0.02,  0.01,  0.00, -0.01, -0.01 },
    { 0.02,  0.02,  0.01,  0.00, -0.01, -0.02, -0.02 },
    { 0.01,  0.01,  0.00, -0.01, -0.01, -0.02, -0.03 },
    { 0.00,  0.00, -0.01, -0.02, -0.02, -0.03, -0.04 },
    { 0.00, -0.01, -0.01, -0.02, -0.03, -0.04, -0.05 }
};

volatile flag_t Flag=
{
        .Left_Ring_Find = false,                    //圆环出口补线标志位
        .Left_Ring_Turn = false,                    //进圆环补线标志位
        .Left_Ring_Out = false,                     //圆环内标志位
        .Left_Ring_Out_Mend1 = false,               //出环固定打角标志位
        .Left_Ring_Out_Mend2 = false,               //出环补线标志位
        .Left_Ring = false,                         //圆环全程 用于屏蔽其他元素处理

        .Right_Ring_Find = false,
        .Right_Ring_Turn = false,
        .Right_Ring_Out = false,
        .Right_Ring_Out_Mend1 = false,
        .Right_Ring_Out_Mend2 = false,
        .Right_Ring = false,
        //十字
        .CrossRoad_Find = true,                    //十字识别标志位
        .CrossRoad_Block = false,                   //十字屏蔽标志位

        .crossRight = false,                  //十字右部分

        .crossLeft = false,                    //十字左部分
        .CrossIn = false,                      //进入十字标志位
        .CrossOut = false,                      //进入十字标志位

        //斑马线
        .Garage_Find = false,
        .Park = false,                              //停车标志位
        .Fan = false,                               //负压风扇标志位
        .GOGOGO = false,                            //发车标志位
        .Motor_Creazy = false,                              //电机疯转标志位
        .Motor_Stem = false,                                //电机堵转标志位
};
/**************************************圆环参数***********************************************/

/**************************************舵机参数***********************************************/
volatile servo_param_t servo_param=
{
        .Servo_filter = 0.0f,
        .cam_servo_temp_1 = 0.0f,
};

/**************************************电机参数***********************************************/

/**************************************控制距离参数***********************************************/
bool flag = 1;
int time0 = 0;
int time1 = 0;
int time2 = 0;
int time3 = 0;
int time4 = 0;
int time5 = 0;
int time6 = 0;
int time7 = 0;
