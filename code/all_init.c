/*
 * all_init.c
 *
 *  Created on: 2023年4月27日
 */

#include "zf_common_headfile.h"



void my_camera_init(void)
{
    while(1)
    {
        if(mt9v03x_init())
            ips200_show_string(0, 1, "mt9v03x reinit.");
       else
            break;
        system_delay_ms(100);                                                   // 短延时快速闪灯表示异常
   }
}

void all_init(void)
{
    // ************** 电机、舵机、蜂鸣器 初始化 ********************************
    beep_init();        // 定时器 3
    MotorInit(2000);    // 定时器 0
    //ServoInit(50);       // 定时器 1
    fan_init(50);
    // ************** 编码器初始化 ************************************
    Encoder_Init();
    // ************* UI 初始化 ********************************
    key_init(5);
    ips200_init(IPS200_TYPE_SPI);
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);    // 显示一个RGB565色彩图片 原图240*80 显示240*80 低位在前
    system_delay_ms(500);
    ips200_full(RGB565_PINK);
    system_delay_ms(500);
    ips200_show_string(0, 0, "rightrightrightright");

    // ************** 摄像头 初始化 ************************************‘
     my_camera_init();


}
