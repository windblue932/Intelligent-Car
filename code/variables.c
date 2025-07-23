    /*
 * variables.c
 *
 *  Created on: 2023��5��11��
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
/************************************������******************************************/

uint8_t send_data_flag = 0;//��־λ����

volatile uint16_t CurrentServoDty = Servo_Center_Mid;          //�������ռ�ձ�


PID motor_dir  = PID_CREATE(1,0,0,1);
PID motor_pid_l = PID_CREATE(35,25,0,1); //����  35,25,0,1
PID motor_pid_r = PID_CREATE(35,25,0,1); //�ҵ��

pid_param_tt motor_pid_turn = PID_CREATETEST(0,0,0);//Сת�����0.5,0,3
pid_param_tt motor_pid_mutu = PID_CREATETEST(2.2,0,5);//��ת�����

pid_param_tt motor_pid_dist = PID_CREATETEST(3.5,0.5,7); //�������   3,0,5   8 0 16

pid_param_tt Servo_dire_pid = PID_CREATETEST(3,0,8);//6.5,0,90   5.5,0,90   2 0 6
pid_param_tt Servo_turn_pid = PID_CREATETEST(3,0,8);//  4.5,0,70   4,0,20
pid_param_tt Servo_mutu_pid = PID_CREATETEST(3,0,8);

float KLI_DEC = 0.1;
float KLO_DEC = 0;
float KRI_DEC = 0.1;   //���ּ���
float KRO_DEC = 0;   //���ּ���

float target_dist = 7;
float base_speed = 195; //185   195
float k_dec = 1;

uint16_t isr_count = 0;
bool flag = 0;
uint32_t time0 = 0;
uint32_t time1 = 0;
uint32_t time2 = 0;

volatile float param_1[11][11] = {0};            //Ԥ������ײ���
volatile float param_2[11][11] = {0};
/************************************PID�ṹ��***********************************************/
volatile flag_t Flag=
{
        .Motor_Creazy = false,                      //�������
        .Motor_Stem = false,                        //�������
        .GOGOGO = false,                            //������־λ
};

