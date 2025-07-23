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

uint8_t iii = 0;
char send_str[32] = {0};

void Iwdg_init(void)
{
    // 获取当前CPU核心的看门狗实例
    Ifx_SCU_WDTCPU *wdt = &MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()];

    // 初始化看门狗配置结构体
    IfxScuWdt_Config wdtConfig;
    IfxScuWdt_initConfig(&wdtConfig); // 加载默认配置

    // 自定义配置参数
    wdtConfig.password           = 0x3C;     // 密码（保持默认0x3C）
    wdtConfig.reload             = 3000;     // 重载值
    wdtConfig.inputFrequency     = IfxScu_WDTCON1_IR_divBy64; // 分频系数64
    wdtConfig.disableWatchdog    = FALSE;    // 使能看门狗

    // 初始化CPU看门狗
    IfxScuWdt_initCpuWatchdog(wdt, &wdtConfig);
}

int core0_main(void)
{
    //Iwdg_init();
    clock_init();               // 获取时钟频率
    debug_init();               // 初始化默认调试串口
    all_init();                 //遥控初始化
    threshold = 230;
    light = 100;
    show_ui();
    mt9v03x_set_exposure_time(light);
    ips200_clear();
    system_delay_ms(500);
    //无线串口初始化
    wireless_uart_init();
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    //示波器
    seekfree_assistant_oscilloscope_struct oscilloscope_data;
    oscilloscope_data.channel_num = 8;
    //图传连接
    //wifi_spi_connect();
    pit_ms_init(CCU60_CH1, 5);      //中断初始化
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        //system_start();
        //flag = 1;
        // 喂狗
        //IfxScuWdt_serviceCpuWatchdog(0x3C); // 使用相同的密码喂狗
/*
        //无线串口调参
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
        //示波器波形查看

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
        //发送数据
        wireless_send_int(imageLine.Point_Center[AIM_LINE]);
        wireless_send_int( image.cam_finalCenterERR[0]);
        wireless_send_int( image.cam_finalCenterERR[1]);
        wireless_send_int( image.cam_finalCenterERR[2]);
        wireless_send_int( image.cam_finalCenterERR[3]);
*/
        //图传发送
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
