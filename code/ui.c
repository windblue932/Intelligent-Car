/*
 * ui.c
 *
 *  Created on: 2025年6月13日
 *      Author: colin
 */

#include "zf_common_headfile.h"

#define Char_num   21  //简单字符个数宏定义

bool UI_FLAG = true;

/*10项一级菜单、10项二级菜单*/
float UI_Param[10][10]={0};

/*菜单界面，简单二级菜单*/
char main_table[][Char_num] =
{
    {"Main Menu"},
    {" Motor     "},
    {" Servo_adc   "},
    {" Speed       "},
    {" PID   "},
    {" Ring   "},
    {" Image    "},
    {          },
    {          },
    {          }
};
char Second_table[][12][Char_num]={
        {//电机
         {"Motor"},
         {" motor_d_kp   "},
         {" motor_d_ki   "},
         {" motor_d_kd   "},
         {" motor_l_kp   "},
         {" motor_l_ki   "},
         {" motor_l_kd   "},
         {" motor_r_kp   "},
         {" motor_r_ki   "},
         {" motor_r_kd   "},
        },
        {//舵机
         {"Servo_adc"},
         {" servo_dire_kp   "},
         {" servo_dire_ki   "},
         {" servo_dire_kd   "},
         {" servo_turn_kp   "},
         {" servo_turn_ki   "},
         {" servo_turn_kd   "},
         {" servo_muta_kp   "},
         {" servo_myta_ki   "},
         {" servo_muta_kd   "},
        },
        {//速度
         {"Speed"},
         {" SPEED_MUTATION   "},
         {" SPEED_TURN       "},
         {" SPEED_STRAIGHT   "},
         {" Purpost_Speed    "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "}
        },

        {//PID
         {"PID"},
         {" KLI_DEC         "},
         {" KLO_DEC         "},
         {" KRI_DEC         "},
         {" KRO_DEC         "},
         {" LOCPID_THRESHOLD"},
         {"                 "},
         {"                 "},
         {"                 "},
         {"                 "}
        },
        {//圆环
         {"Ring"},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
         {"              "},
        },
        {//图像
         {"Image"},
         {" AIM_LINE_STRA     "},
         {" AIM_LINE_TURN     "},
         {" AIM_LINE_MUTA     "},
         {" light             "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
        },
};


void param_read(void)
{
        //**********电机参数**************
        UI_Param[0][0] = motor_pid_d.Kp;
        UI_Param[0][1] = motor_pid_d.Ki;
        UI_Param[0][2] = motor_pid_d.Kd;
        UI_Param[0][3] = motor_pid_l.Kp;
        UI_Param[0][4] = motor_pid_l.Ki;
        UI_Param[0][5] = motor_pid_l.Kd;
        UI_Param[0][6] = motor_pid_r.Kp;
        UI_Param[0][7] = motor_pid_r.Ki;
        UI_Param[0][8] = motor_pid_r.Kd;

        //**********舵机参数**************
        UI_Param[1][0] = servo_pid_dire.kp;
        UI_Param[1][1] = servo_pid_dire.ki;
        UI_Param[1][2] = servo_pid_dire.kd;
        UI_Param[1][3] = servo_pid_turn.kp;
        UI_Param[1][4] = servo_pid_turn.ki;
        UI_Param[1][5] = servo_pid_turn.kd;
        UI_Param[1][6] = servo_pid_muta.kp;
        UI_Param[1][7] = servo_pid_muta.ki;
        UI_Param[1][8] = servo_pid_muta.kd;

        //**********速度*****************
        UI_Param[2][0] = SPEED_MUTATION;
        UI_Param[2][1] = SPEED_TURN;
        UI_Param[2][2] = SPEED_STRAIGHT;
        UI_Param[2][3] = Purpost_Speed;

        //**********PID*****************
        UI_Param[3][0] = KLI_DEC;
        UI_Param[3][1] = KLO_DEC;
        UI_Param[3][2] = KRI_DEC;
        UI_Param[3][3] = KRO_DEC;
        UI_Param[3][4] = LOCPID_THRESHOLD;

        //**********圆环*****************

        //**********图像*****************
        UI_Param[5][0] = AIM_LINE_STRAIGHT;
        UI_Param[5][1] = AIM_LINE_TURN;
        UI_Param[5][2] = AIM_LINE_MUTATION;
        UI_Param[5][3] = light;

        return;

}


void param_save(void)
{
        //**********电机参数**************
        motor_pid_d.Kp = UI_Param[0][0];
        motor_pid_d.Ki = UI_Param[0][1];
        motor_pid_d.Kd = UI_Param[0][2];
        motor_pid_l.Kp = UI_Param[0][3];
        motor_pid_l.Ki = UI_Param[0][4];
        motor_pid_l.Kd = UI_Param[0][5];
        motor_pid_r.Kp = UI_Param[0][6];
        motor_pid_r.Ki = UI_Param[0][7];
        motor_pid_r.Kd = UI_Param[0][8];

        //**********舵机参数**************
        servo_pid_dire.kp = UI_Param[1][0];
        servo_pid_dire.ki = UI_Param[1][1];
        servo_pid_dire.kd = UI_Param[1][2];
        servo_pid_turn.kp = UI_Param[1][3];
        servo_pid_turn.ki = UI_Param[1][4];
        servo_pid_turn.kd = UI_Param[1][5];
        servo_pid_muta.kp = UI_Param[1][6];
        servo_pid_muta.ki = UI_Param[1][7];
        servo_pid_muta.kd = UI_Param[1][8];

        //**********速度*****************
        SPEED_MUTATION = UI_Param[2][0];
        SPEED_TURN = UI_Param[2][1];
        SPEED_STRAIGHT = UI_Param[2][2];
        Purpost_Speed = UI_Param[2][3];

        //**********PID*****************
        KLI_DEC = UI_Param[3][0];
        KLO_DEC = UI_Param[3][1];
        KRI_DEC = UI_Param[3][2];
        KRO_DEC = UI_Param[3][3];
        LOCPID_THRESHOLD = UI_Param[3][4];

        //**********圆环*****************

        //**********图像*****************
        AIM_LINE_STRAIGHT = UI_Param[5][0];
        AIM_LINE_TURN = UI_Param[5][1];
        AIM_LINE_MUTATION = UI_Param[5][2];
        light = UI_Param[5][3];

        return;

}


void select_row(int row)
{

        static int last_row1 = 1;
        int row_height = 20;  // 每行间距，防止贴在一起
        int x_offset = 0;    // x 坐标偏移，保持图标位置一致

        // 清除上一个选择的行（显示空白图标）
        ips200_show_string(x_offset, (last_row1 + 1)* row_height, "  ");

        // 在新行上绘制选中图标
        ips200_show_string(x_offset, (row + 1)* row_height,">>");

        // 显示当前选中的行数，确保它不会被图标覆盖
        ips200_show_int(0, 0, row, 1);

        // 记录当前行
        last_row1 = row;
}

void select_row2(int row)
{

        static int last_row2 = 1;
        int y_offset  =  30;
        int row_height = 20;  // 每行间距，防止贴在一起
        int x_offset = 0;    // x 坐标偏移，保持图标位置一致

        // 清除上一个选择的行（显示空白图标）
        ips200_show_string(x_offset, last_row2 * row_height + y_offset, "  ");

        // 在新行上绘制选中图标
        ips200_show_string(x_offset, row * row_height + y_offset, ">>");

        // 显示当前选中的行数，确保它不会被图标覆盖
        ips200_show_int(0, 0, row, 1);

        // 记录当前行
        last_row2 = row;
}



/*****************************************************************
*Function: 显示主菜单
*Description: 显示主菜单（即一级菜单）界面，为了滚动显示效果，需要设定从
*Description: 主菜单列表的哪一行开始显示
*Return:
*****************************************************************/
void show_main_window(uint8 line)
{

       uint16 x = 20, y = 20;  // 初始显示位置

       /*for(int i = 0; i < sizeof(main_table)/sizeof(main_table[0]); i++)
       {
           ips200_show_string(x, y, main_table[i]);
           y += 20;  // 每行间隔 20 像素，避免字符重叠
       }
        */

    ips200_show_string(x, y, (const char*)main_table[0]);
    y += 20;

    for(uint8 i=0;i<9;i++)
    {
        ips200_show_string(x,y,(const char*)main_table[1+i+line]);
        y+=20;
    }

}

/*****************************************************************
*Function: 显示副菜单
*Description: 显示副菜单（即二级菜单）界面
*parameter:  List 哪一个二级菜单，line 从二级菜单的哪一行开始显示
*Return:
*****************************************************************/
void show_second(uint8_t List,uint8_t line)
{

        uint16 x = 20, y = 20;  // 调整起始坐标，避免过于靠近左上角

        // 显示主标题
        //ips200_show_int(0, 100, List,1);
        ips200_show_string(x, y, (char*)Second_table[List - 1][0]);
        y += 30;  // 标题和列表之间留出间隔

        // 显示菜单内容
        for (uint8 i = 0; i < 9; i++)
        {
            ips200_show_string(x, y, (char*)Second_table[List - 1][1 + i + line]);  // 文本
            ips200_show_float(x + 150, y, UI_Param[List - 1][i + line], 4, 2);      // 数值参数
            y += 20;  // 每行间隔 20 像素，防止重叠
        }
}

/*****************************************************************
*Function: 显示调整参数
*Description: 显示调参界面
*parameter:  List 哪一个二级菜单，line 显示哪个参数
*Return:
*****************************************************************/
void show_variable(uint8_t List,uint8_t line)
{

     uint16 x = 10, y = 100;  // 调整起始坐标，避免过于靠近左上角

     ips200_show_string(x, y, (char*)Second_table[List - 1][line]);  // 文本
     ips200_show_float(x + 150, y, UI_Param[List - 1][line - 1], 4, 2);      // 数值参数

}

/*****************************************************************
*Function: 显示加载预设参数
*Description: 显示加载预设参数
*parameter:
*Return:
*****************************************************************/
void show_load(void)
{
     ips200_show_string(10, 20,"Select to load variables:");  // 文本
     ips200_show_string(10, 60,"1 High speed");  // 文本
     ips200_show_string(10, 160,"2 Low speed");  // 文本
     //ips200_show_chinese(10, 20, 16, "请选择要加载的预设参数：", 20, RGB565_RED);
     //ips200_show_chinese(10, 60, 16, "1高速", 5, RGB565_RED);
     //ips200_show_chinese(110, 60, 16, "2低速", 5, RGB565_RED);
}

/*****************************************************************
*Function: 一级菜单下显示被选中行
*Description: 反转显示被选中的那一行
*parameter:
*Return:
*****************************************************************/
uint8_t cnt = 0;  // 记录当前窗口起始行
int8_t show_change(void)
{
    static int8_t LINE = 1;  // 记录当前选中行
    int8_t MAX_LINE = 9;     // 最大行数（假设菜单最多 9 行）

    // 处理按键输入
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // 上移
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < MAX_LINE)
    {
        LINE++;  // 下移
    }
/*
    // 处理菜单翻页逻辑
    if (LINE - cnt >= 8)  // 滚动窗口向下翻页
    {
        cnt++;
    }
    else if (LINE - cnt < 0)  // 滚动窗口向上翻页
    {
        cnt--;
    }
*/
    // 更新选中行的显示
    show_main_window(cnt);
    select_row(LINE - cnt);

    key_clear_all_state();  // 清除按键状态
    key_scanner();  // 扫描按键

    return LINE;
}


/*****************************************************************
*Function: 二级菜单下显示被选中行
*Description: 反转显示被选中的那一行
*parameter: line 哪个二级菜单
*Return:
*****************************************************************/
static uint8_t cnt2 = 0;  // 记录二级菜单的起始行
uint8_t show_change2(uint8_t line)
{
    static uint8_t LINE_a = 1;       // 记录当前选中的行
    static uint8_t line_last = 1;  // 记录上次的菜单，避免重复显示同一菜单

    // 如果当前菜单与上次菜单不同，重新从头开始显示
    if (line_last != line)
    {
        LINE_a = 1;  // 重置为第一行
        cnt2 = 0;  // 重置菜单偏移
    }
    line_last = line;

    // 处理按键输入
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE_a > 1)
    {
        LINE_a--;  // 上移
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE_a < 9)  // 假设最大行数为9
    {
        LINE_a++;  // 下移
    }
/*
    // 处理菜单翻页
    if (LINE_a - cnt2 >= 8)  // 当前行超出菜单范围，向下翻页
    {
        cnt2++;
    }
    else if (LINE_a - cnt2 < 0)  // 当前行超出菜单范围，向上翻页
    {
        cnt2--;
    }
*/
    // 更新选中行的显示
    show_second(line, cnt2);
    select_row2(LINE_a - cnt2);

    key_clear_all_state();  // 清除按键状态
    key_scanner();  // 扫描按键

    return LINE_a;  // 返回当前选中的行
}

/*****************************************************************
*Function: 调参函数
*Description: 反转显示被选中的那一行
*parameter:line 哪个二级菜单,LINE 哪个值, rate 显示调节步长
*Return:
*****************************************************************/
void show_change3(uint8_t line, uint8_t LINE, float rate)
{

    if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
    {
        UI_Param[line - 1][LINE - 1] += rate;  // 增加值
    }
    else if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
    {
        UI_Param[line - 1][LINE - 1] -= rate;  // 减少值
    }



    // 显示当前调节的步长和参数值

    ips200_show_float(90, 0, rate, 4, 3);
    show_variable(line,LINE);

    key_clear_all_state();  // 清除按键状态
    key_scanner();  // 扫描按键

    return;
}

/*****************************************************************
*Function: 步长速率转换
*Description:
*parameter: cnt 1-5，共六个步长单位
*Return:
*****************************************************************/
float step_rate(uint8_t cnt)
{
    if(cnt%6==1)      return 1.0f;
    else if(cnt%6==2) return 10.0f;
    else if(cnt%6==3) return 100.0f;
    else if(cnt%6==4) return 1000.0f;
    else if(cnt%6==5) return 0.01f;
    else              return 0.1f;

}


void show_ui(void)
{
    key_clear_all_state();
    int wave_flag = 0;          //状态机
    uint8_t precision = 1;      //调参精度
    float rate = 1;             //调参步长
    bool loop_flag = true;      //循环标志位
    int16_t LINE_1 = 1;         //一级菜单选中的行
    int16_t LINE_2 = 1;         //二级菜单选中的行
    int i,j = 0;


    // ********* 显示封面 ******************
    /*
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);
    system_delay_ms(1000);
    ips200_clear();
    */
    ips200_show_string(0, 0, "UESTC");
    system_delay_ms(1000);

    param_read();

    // 开始 UI 调参
    ips200_clear();
    show_main_window(0);
    select_row(1);



    while(loop_flag)
    {
        //***********长按4退出ui********************
        if(key_get_state(KEY_4) == KEY_LONG_PRESS)
        {
            param_save();
            //UI_FLAG = false;
            loop_flag = false;
            break;
        }


        switch(wave_flag)
        {

            //**************一级菜单**************************
            //***********2，3键滚动选择，1键确认，4键去预设参数界面*****************
            case 0 :
                LINE_1 = show_change();
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                {
                    wave_flag = 3;
                    ips200_clear();
                    system_delay_ms(10);
                }
                else if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                {
                    wave_flag = 1;
                    ips200_clear();
                }
                break;


            //**************二级菜单**************************
            //***********2，3键滚动选择，1键确认，4键退出去一级菜单*****************
            case 1 :
                LINE_2 = show_change2(LINE_1);
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                {
                    wave_flag = 0;
                    ips200_clear();
                }
                else if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                {
                    wave_flag = 2;
                    ips200_clear();
                }
                break;



            //**************调参界面**************************
            //***********2，3键加减值，1键改步长，4键退出去二级菜单*****************
            case 2 :
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                {
                    precision++;
                    if(precision > 6)
                        precision = 1;
                }
                rate = step_rate(precision);
                show_change3(LINE_1, LINE_2, rate);
                if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                {
                    wave_flag = 1;
                    ips200_clear();
                }
                break;


            //**************加载预设参数界面**************************
            //***********1，2键分别选两套参数，4键退出去一级菜单*****************
            case 3 :
                key_clear_all_state();
                key_scanner();

                show_load();
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                {
                    for(i = 0; i < 10; i++)
                    {
                        for(j = 0; j < 10; j++)
                        {
                            UI_Param[i][j] = param_1[i][j];
                        }
                    }
                    param_save();
                    ips200_show_string(120, 60,"Load Success");  // 文本
                    system_delay_ms(500);
                    ips200_show_string(120, 60,"            ");
                    //loop_flag = false;
                }
                else if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
                {
                    for(i = 0; i < 10; i++)
                    {
                        for(j = 0; j < 10; j++)
                        {
                            UI_Param[i][j] = param_2[i][j];
                        }
                    }
                    param_save();
                    ips200_show_string(120, 160,"Load Success");  // 文本
                    system_delay_ms(500);
                    ips200_show_string(120, 160,"            ");
                    //loop_flag = false;
                }

                if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                {
                    wave_flag = 0;
                    ips200_clear();
                }
                break;

            default :
                wave_flag = 0;
                ips200_clear();

        }

    }

    UI_FLAG = false;
    ips200_clear();
    system_delay_ms(100);
}




