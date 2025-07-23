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

uint8_t iii = 0;
char send_str[32] = {0};

void Iwdg_init(void)
{
    // ��ȡ��ǰCPU���ĵĿ��Ź�ʵ��
    Ifx_SCU_WDTCPU *wdt = &MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()];

    // ��ʼ�����Ź����ýṹ��
    IfxScuWdt_Config wdtConfig;
    IfxScuWdt_initConfig(&wdtConfig); // ����Ĭ������

    // �Զ������ò���
    wdtConfig.password           = 0x3C;     // ���루����Ĭ��0x3C��
    wdtConfig.reload             = 3000;     // ����ֵ
    wdtConfig.inputFrequency     = IfxScu_WDTCON1_IR_divBy64; // ��Ƶϵ��64
    wdtConfig.disableWatchdog    = FALSE;    // ʹ�ܿ��Ź�

    // ��ʼ��CPU���Ź�
    IfxScuWdt_initCpuWatchdog(wdt, &wdtConfig);
}

int core0_main(void)
{
    //Iwdg_init();
    clock_init();               // ��ȡʱ��Ƶ��
    debug_init();               // ��ʼ��Ĭ�ϵ��Դ���
    all_init();                 //ң�س�ʼ��
    threshold = 230;
    light = 100;
    show_ui();
    mt9v03x_set_exposure_time(light);
    ips200_clear();
    system_delay_ms(500);
    //���ߴ��ڳ�ʼ��
    wireless_uart_init();
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    //ʾ����
    seekfree_assistant_oscilloscope_struct oscilloscope_data;
    oscilloscope_data.channel_num = 8;
    //ͼ������
    //wifi_spi_connect();
    pit_ms_init(CCU60_CH1, 5);      //�жϳ�ʼ��
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        //system_start();
        //flag = 1;
        // ι��
        //IfxScuWdt_serviceCpuWatchdog(0x3C); // ʹ����ͬ������ι��
/*
        //���ߴ��ڵ���
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
        motor_pid_direction.kp = seekfree_assistant_parameter[0];
        motor_pid_direction.ki = seekfree_assistant_parameter[1];
        motor_pid_direction.kd = seekfree_assistant_parameter[2];
        K_DEC = seekfree_assistant_parameter[3];
*/

        ips200_show_gray_image(130, 10, C_Image[0], ImageW, ImageH, ImageW, ImageH, 0);
        ips200_show_gray_image(25, 10, C_Image[0], ImageW, ImageH,ImageW,ImageH, bin_thr);

        show_line();

        ips200_show_string(10, 80, "Inv_Row: ");
        ips200_show_int(80, 80, In_Down_Point_Row, 2);
        ips200_show_string(100, 80, "Inv_Col: ");
        ips200_show_int(170, 80, In_Down_Point, 2);
        ips200_show_string(10, 100, "Straight_Flag: ");
        ips200_show_int(180, 100, Straight_Flag, 2);
        ips200_show_string(10, 120, "Turn_Flag: ");
        ips200_show_int(180, 120, Turn_Flag, 2);
        ips200_show_string(10, 140, "Mutation_Flag: ");
        ips200_show_int(180, 140, Mutation_Flag, 2);
        ips200_show_string(10, 160, "servo_filter: ");
        ips200_show_float(180, 160, servo_param.Servo_filter, 2, 3);
        ips200_show_string(10, 180, "L_s:");
        ips200_show_int(60, 180, Left_Island_state, 3);
        ips200_show_string(10, 200, "L_f0:");
        ips200_show_int(60, 200, Left_Island_flag0, 3);
        ips200_show_string(10, 220, "L_f1:");
        ips200_show_int(60, 220, Left_Island_flag1, 3);
        ips200_show_string(10, 240, "L_f2:");
        ips200_show_int(60, 240, Left_Island_flag2, 3);
        ips200_show_string(10, 260, "L_f3:");
        ips200_show_int(60, 260, Left_Island_flag3, 3);
        ips200_show_string(10, 280, "L_f4:");
        ips200_show_int(60, 280, Left_Island_flag4, 3);
        ips200_show_string(80, 180, "R_s:");
        ips200_show_int(130, 180, Right_Island_state, 3);
        ips200_show_string(80, 200, "R_f0:");
        ips200_show_int(130, 200, Right_Island_flag0, 3);
        ips200_show_string(80, 220, "R_f1:");
        ips200_show_int(130, 220, Right_Island_flag1, 3);
        ips200_show_string(80, 240, "R_f2:");
        ips200_show_int(130, 240, Right_Island_flag2, 3);
        ips200_show_string(80, 260, "R_f3:");
        ips200_show_int(130, 260, Right_Island_flag3, 3);
        ips200_show_string(80, 280, "R_f4:");
        ips200_show_int(130, 280, Right_Island_flag4, 3);
        ips200_show_string(150, 180, "IDF:");
        ips200_show_int(200, 180, In_Down_Flag, 3);
        ips200_show_string(150, 200, "IUF:");
        ips200_show_int(200, 200, In_Up_Point_Flag, 3);
        ips200_show_string(150, 220, "ODF:");
        ips200_show_int(200, 220, Out_Down_Point_Flag, 3);
        ips200_show_string(150, 240, "SIDF:");
        ips200_show_int(200, 240, Search_In_Down_Point_Flag, 3);

/*
        ips200_show_string(10, 100, "Flag.Park: ");
        ips200_show_int(180, 100, Flag.Park, 2);
        ips200_show_string(10, 120, "Flag.Motor_Creazy: ");
        ips200_show_int(180, 120, Flag.Motor_Creazy, 2);
        ips200_show_string(10, 140, "Flag.Motor_Stem: ");
        ips200_show_int(180, 140, Flag.Motor_Stem, 2);
        ips200_show_string(10, 160, "servo_filter: ");
        ips200_show_int(180, 160, servo_param.Servo_filter, 2);
        ips200_show_string(10, 180, "left_speed: ");
        ips200_show_int(180, 180, motor_l.target_speed, 5);
        ips200_show_string(10, 200, "right_speed: ");
        ips200_show_int(180, 200, motor_r.target_speed, 5);
        ips200_show_string(10, 220, "aim_line: ");
        ips200_show_int(180, 220, AIM_LINE, 5);
*/
/*
        //ʾ�������β鿴

        oscilloscope_data.data[0] = motor_l.encoder_speed;
        oscilloscope_data.data[1] = motor_r.encoder_speed;
        oscilloscope_data.data[2] = Flag.Motor_Stem;
        oscilloscope_data.data[3] = Straight_Flag;
        oscilloscope_data.data[4] = Turn_Flag;
        oscilloscope_data.data[5] = Mutation_Flag;
        oscilloscope_data.data[6] = servo_param.Servo_filter;
        oscilloscope_data.data[7] = difference_speed;
        seekfree_assistant_oscilloscope_send(&oscilloscope_data);
*/

        /*
        oscilloscope_data.data[0] = motor_l.duty;
        oscilloscope_data.data[1] = motor_r.duty;
        oscilloscope_data.data[2] = Straight_Flag;
        oscilloscope_data.data[3] = motor_l.target_speed;
        oscilloscope_data.data[4] = motor_r.target_speed;
        oscilloscope_data.data[5] = servo_param.Servo_filter;
        oscilloscope_data.data[6] = motor_l.encoder_speed;
        oscilloscope_data.data[7] = motor_r.encoder_speed;
        seekfree_assistant_oscilloscope_send(&oscilloscope_data);
        */

/*
        oscilloscope_data.data[0] = time0;
        oscilloscope_data.data[1] = time1 - time0;
        oscilloscope_data.data[2] = time2 - time1;
        oscilloscope_data.data[3] = time3 - time2;
        oscilloscope_data.data[4] = time4 - time3;
        oscilloscope_data.data[5] = time5 - time4;
        oscilloscope_data.data[6] = time6 - time5;
        oscilloscope_data.data[7] = time7 - time6;
        seekfree_assistant_oscilloscope_send(&oscilloscope_data);
*/
/*
        oscilloscope_data.data[0] = In_Down_Point_Row;
        oscilloscope_data.data[1] = In_Down_Point;
        oscilloscope_data.data[2] = Inv_Point_Row[0];
        oscilloscope_data.data[3] = Inv_Point[0];
        oscilloscope_data.data[4] = In_Up_Point_Row;
        oscilloscope_data.data[5] = In_Up_Point;
        oscilloscope_data.data[6] = Out_Down_Point_Row;
        oscilloscope_data.data[7] = Out_Down_Point;
        seekfree_assistant_oscilloscope_send(&oscilloscope_data);
*/
/*
        //��������
        wireless_send_int(imageLine.Point_Center[AIM_LINE]);
        wireless_send_int( image.cam_finalCenterERR[0]);
        wireless_send_int( image.cam_finalCenterERR[1]);
        wireless_send_int( image.cam_finalCenterERR[2]);
        wireless_send_int( image.cam_finalCenterERR[3]);
*/
        //ͼ������
        //wifi_spi_send();
        /*
        if(flag == 1){
            time0 = system_getval_us();
            flag = 0;
        }
        */
    }
}
#pragma section all restore
