
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
* �ļ�����          cpu0_main
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
#pragma section all "cpu0_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��
uint8_t iii = 0;
char send_str[32] = {0};
int core0_main(void)
{
    clock_init();                   // ��ȡʱ��Ƶ��<��ر���>
    debug_init();                   // ��ʼ��Ĭ�ϵ��Դ���
    all_init();
    light = 220;
    show_ui();
    mt9v03x_set_exposure_time(light);
    ips200_clear();
    //�жϳ�ʼ��
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
           // ���±�־λ
           if(seekfree_assistant_parameter_update_flag[iii])
           {
               seekfree_assistant_parameter_update_flag[iii] = 0;
               // ͨ������ת���ڷ�����Ϣ
               sprintf(send_str,"receive data channel : %d ", iii);
               wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
               sprintf(send_str,"data : %.2f \r\n", seekfree_assistant_parameter[iii]);
               wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
           }
       }
           //������ֵ��
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
