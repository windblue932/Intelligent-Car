    /*
 * variables.c
 *
 *  Created on: 2023��5��11��
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
/************************************������******************************************/
int pid_down_left_limit = 8000;  //��������
int pid_up_left_limit = 5000;    //��������
int pid_down_right_limit = 5500; //��������
int pid_up_right_limit = 5000;   //��������
/************************************�Ӿ��������******************************************/
int AIM_LINE_STRAIGHT = 10;
int AIM_LINE_TURN = 20;
int AIM_LINE_MUTATION = 20;

/************************************�����������***********************************************/
PID motor_pid_d = PID_CREATE(1, 0, 0, 1);    //ֱ���
PID motor_pid_l = PID_CREATE(40, 20, 0, 1);    //����    ��25 10 5 8��  (45, 11, 5, 1)  45, 13, 5, 1  40, 18, 0, 1    45, 28, 0, 1
PID motor_pid_r = PID_CREATE(40, 20, 0, 1);    //�ҵ��

pid_param_tt servo_pid_dire=PID_CREATETEST(2, 0, 5);     //ֱ�����
pid_param_tt servo_pid_turn=PID_CREATETEST(3.5, 0, 7);        //С����    //7.4����13.00��6 0 15�����ϵ�
pid_param_tt servo_pid_muta=PID_CREATETEST(3.5, 0, 7);        //������

//��ʼ����
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

volatile float param_1[10][10] = {0};            //Ԥ������ײ���
volatile float param_2[10][10] = {0};

//��Kpģ��������
float fuzzy_Kp[7][7] = {
    { 0.2,  0.2,  0.1,  0.1,  0.0,  0.0,  0.0 },
    { 0.2,  0.1,  0.1,  0.1,  0.0,  0.0, -0.1 },
    { 0.1,  0.1,  0.1,  0.0,  0.0, -0.1, -0.1 },
    { 0.1,  0.1,  0.0,  0.0,  0.0, -0.1, -0.1 },
    { 0.1,  0.0,  0.0,  0.0, -0.1, -0.1, -0.1 },
    { 0.0,  0.0, -0.1, -0.1, -0.1, -0.2, -0.2 },
    { 0.0, -0.1, -0.1, -0.1, -0.2, -0.2, -0.2 }
};

//��Kiģ��������
float fuzzy_Ki[7][7] = {
    { 0.02,  0.02,  0.01,  0.01,  0.00,  0.00,  0.00 },
    { 0.02,  0.01,  0.01,  0.01,  0.00,  0.00, -0.01 },
    { 0.01,  0.01,  0.01,  0.00,  0.00, -0.01, -0.01 },
    { 0.01,  0.01,  0.00,  0.00,  0.00, -0.01, -0.01 },
    { 0.01,  0.00,  0.00,  0.00, -0.01, -0.01, -0.01 },
    { 0.00,  0.00, -0.01, -0.01, -0.01, -0.02, -0.02 },
    { 0.00, -0.01, -0.01, -0.01, -0.02, -0.02, -0.02 }
};

//��Kdģ��������
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
        .Left_Ring_Find = false,                    //Բ�����ڲ��߱�־λ
        .Left_Ring_Turn = false,                    //��Բ�����߱�־λ
        .Left_Ring_Out = false,                     //Բ���ڱ�־λ
        .Left_Ring_Out_Mend1 = false,               //�����̶���Ǳ�־λ
        .Left_Ring_Out_Mend2 = false,               //�������߱�־λ
        .Left_Ring = false,                         //Բ��ȫ�� ������������Ԫ�ش���

        .Right_Ring_Find = false,
        .Right_Ring_Turn = false,
        .Right_Ring_Out = false,
        .Right_Ring_Out_Mend1 = false,
        .Right_Ring_Out_Mend2 = false,
        .Right_Ring = false,
        //ʮ��
        .CrossRoad_Find = true,                    //ʮ��ʶ���־λ
        .CrossRoad_Block = false,                   //ʮ�����α�־λ

        .crossRight = false,                  //ʮ���Ҳ���

        .crossLeft = false,                    //ʮ���󲿷�
        .CrossIn = false,                      //����ʮ�ֱ�־λ
        .CrossOut = false,                      //����ʮ�ֱ�־λ

        //������
        .Garage_Find = false,
        .Park = false,                              //ͣ����־λ
        .Fan = false,                               //��ѹ���ȱ�־λ
        .GOGOGO = false,                            //������־λ
        .Motor_Creazy = false,                              //�����ת��־λ
        .Motor_Stem = false,                                //�����ת��־λ
};
/**************************************Բ������***********************************************/

/**************************************�������***********************************************/
volatile servo_param_t servo_param=
{
        .Servo_filter = 0.0f,
        .cam_servo_temp_1 = 0.0f,
};

/**************************************�������***********************************************/

/**************************************���ƾ������***********************************************/
bool flag = 1;
int time0 = 0;
int time1 = 0;
int time2 = 0;
int time3 = 0;
int time4 = 0;
int time5 = 0;
int time6 = 0;
int time7 = 0;
