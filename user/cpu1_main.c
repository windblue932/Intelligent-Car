/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          cpu1_main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.10.2
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��

void core1_main(void) {
    disable_Watchdog();                     // �رտ��Ź�
    interrupt_global_enable(0);             // ��ȫ���ж�
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        if (mt9v03x_finish_flag)//mt9v03x_finish_flag
        {
            mt9v03x_finish_flag = 0;
            flag = 1;
            system_start();
            /******************ͼ����*******************************/
            ImageProcess();
            if(flag == 1) time0 = system_getval_us();
            Get_White_Num();
            if(flag == 1) time1 = system_getval_us();
            trackDFS();
            if(flag == 1) time2 = system_getval_us();
            Left_Right_Confusion_Filter();
            if(flag == 1) time3 = system_getval_us();
            Second_Defference();
            if(flag == 1) time4 = system_getval_us();
            Longest_White_Column();
            if(flag == 1) time5 = system_getval_us();
            if(Flag.Park || Flag.Motor_Creazy || Flag.Motor_Stem) //
            {
                pwm_set_duty(FAN_PWM_LEFT, 0);
                pwm_set_duty(FAN_PWM_RIGT, 0);
                Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,0,0,1);
                Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,0,0,1);
                pit_all_close();
            }
            Left_Island_detect();
            Right_Island_detect();
            if(flag == 1) time6 = system_getval_us();

            if(Left_Island_state && !Right_Island_state)
            {
                doFilter();
                Left_Island_logic();
                Left_Island_update();
                beep_set_sound(BEEP_ON);
            }
            else if(Right_Island_state && !Left_Island_state)
            {
                doFilter();
                Right_Island_logic();
                Right_Island_update();
                beep_set_sound(BEEP_ON);
            }
            else
            {
                doFilter();
                doMend();
                updateMediumLine();
                //zebra_found_zz();
                beep_set_sound(BEEP_OFF);
            }
            if(flag == 1){
                time7 = system_getval_us();
                flag = 0;
            }

        }
    }
    pit_all_close();
}
#pragma section all restore
