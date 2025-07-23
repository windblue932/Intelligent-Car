/*
 * all_init.c
 *
 *  Created on: 2023��4��27��
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
        system_delay_ms(100);                                                   // ����ʱ�������Ʊ�ʾ�쳣
   }
}

void all_init(void)
{
    // ************** ���������������� ��ʼ�� ********************************
    beep_init();        // ��ʱ�� 3
    MotorInit(2000);    // ��ʱ�� 0
    //ServoInit(50);       // ��ʱ�� 1
    fan_init(50);
    // ************** ��������ʼ�� ************************************
    Encoder_Init();
    // ************* UI ��ʼ�� ********************************
    key_init(5);
    ips200_init(IPS200_TYPE_SPI);
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);    // ��ʾһ��RGB565ɫ��ͼƬ ԭͼ240*80 ��ʾ240*80 ��λ��ǰ
    system_delay_ms(500);
    ips200_full(RGB565_PINK);
    system_delay_ms(500);
    ips200_show_string(0, 0, "rightrightrightright");

    // ************** ����ͷ ��ʼ�� ************************************��
     my_camera_init();


}
