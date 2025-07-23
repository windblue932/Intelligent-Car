/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu1_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中

void core1_main(void) {
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        if (mt9v03x_finish_flag)//mt9v03x_finish_flag
        {
            mt9v03x_finish_flag = 0;
            flag = 1;
            system_start();
            /******************图像处理*******************************/
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
