
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
* 文件名称          cpu0_main
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
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中
uint8_t iii = 0;
char send_str[32] = {0};
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    all_init();
    light = 220;
    show_ui();
    mt9v03x_set_exposure_time(light);
    ips200_clear();
    //中断初始化
    //wifi_spi_connect();
    wireless_uart_init();
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    seekfree_assistant_oscilloscope_struct oscilloscope_data;
    oscilloscope_data.channel_num = 8;
    system_delay_ms(500);
    pit_ms_init(CCU61_CH0, 5);
    cpu_wait_event_ready();
    while (TRUE)
    {
       //ips200_show_gray_image(26, 10, mt9v03x_image, ImageW, ImageH, ImageW, ImageH, bin_thr);
       //ips200_show_binary_image(26, 10, Pixle, ImageW, ImageH, ImageW, ImageH);
       for(uint8_t i = 0; i < ImageH; i++)
       {
           for(uint8_t j = 0; j < ImageW; j++)
           {
               if(Pixle[i][j]) ips200_draw_point(j + 26, i + 10, RGB565_WHITE);
               else ips200_draw_point(j + 26, i + 10, RGB565_BLACK);
           }
       }

       ips200_show_string(10, 130, "1_xx: ");
       ips200_show_float(80, 130, img1_xx,3,2);
       //ips200_show_string(120, 130, "2_xx: ");
       //ips200_show_float(150, 130, img2_xx,3,2);

       ips200_show_string(10, 150, "img1_yy: ");
       ips200_show_float(80, 150, img1_yy,2,2);
       ips200_show_string(120, 150, "img2_yy: ");
       ips200_show_float(190, 150, img2_yy,2,2);

       ips200_show_string(10, 170, "centerline: ");
       ips200_show_float(80, 170, center_line,2,2);
       ips200_show_string(120, 170, "delta_y: ");
       ips200_show_float(190, 170, delta_y,2,2);

       ips200_show_string(10, 190, "_y ");
       ips200_show_float(80, 190, variance_y,2,2);

       ips200_show_string(10, 210, "loss");
       ips200_show_int(80, 210, count_stop_loss,3);
       ips200_show_string(120, 210, "near");
       ips200_show_int(190, 210, count_stop_near,3);

       /*key_clear_all_state();
       key_scanner();
       if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
           threshold += 5;
       if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
           threshold -= 5;
       if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
           light += 50;
       if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
           light -= 50;*/

       oscilloscope_data.data[0] = motorerror;
       oscilloscope_data.data[1] = difference_speed;
       oscilloscope_data.data[2] = deltaspeed;
       oscilloscope_data.data[3] = delta_y;
       oscilloscope_data.data[4] = motor_l.encoder_speed;
       oscilloscope_data.data[5] = motor_r.encoder_speed;
       oscilloscope_data.data[6] = motor_l.target_speed;
       oscilloscope_data.data[7] = motor_r.target_speed;
       seekfree_assistant_oscilloscope_send(&oscilloscope_data);
        /*
       seekfree_assistant_data_analysis();
       for(uint8_t iii = 0; iii < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; iii++)
       {
           // 更新标志位
           if(seekfree_assistant_parameter_update_flag[iii])
           {
               seekfree_assistant_parameter_update_flag[iii] = 0;
               // 通过无线转串口发送信息
               sprintf(send_str,"receive data channel : %d ", iii);
               wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
               sprintf(send_str,"data : %.2f \r\n", seekfree_assistant_parameter[iii]);
               wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
           }
       }
           //参数赋值：
       pwm_output = seekfree_assistant_parameter[0];
       //motor_pid_direction.ki = seekfree_assistant_parameter[1];
       //motor_pid_direction.kd = seekfree_assistant_parameter[2];
       //K_DEC = seekfree_assistant_parameter[3];
       */

       //wifi_spi_send();

    }
    return 0;
}

#pragma section all restore
