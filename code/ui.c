/*
 * ui.c
 *
 *  Created on: 2025��6��13��
 *      Author: colin
 */

#include "zf_common_headfile.h"

#define Char_num   25  //���ַ������궨��

bool UI_FLAG = true;

/*10��һ���˵���10������˵�*/
float UI_Param[11][11]={0};

/*�˵����棬�򵥶����˵�*/
char main_table[][Char_num] =
{
    {"Main Menu"},
    {" Motor     "},
    {" Servo_adc   "},
    {" Speed       "},
    {" Diffirence_Speed "},
    {" Distance_control "},
    {" Image    "},
};
char Second_table[][12][Char_num]={
        {//���
         {"Motor"},
         {" motor_l_kp   "},
         {" motor_l_ki   "},
         {" motor_l_kd   "},
         {" motor_r_kp   "},
         {" motor_r_ki   "},
         {" motor_r_kd   "},
         {"              "},
         {"              "},
         {"              "},
         {"              "}
        },
        {//���
         {"Servo_adc"},
         {" Servo_dire_pid_kp"},
         {" Servo_dire_pid_ki"},
         {" Servo_dire_pid_kd"},
         {" Servo_turn_pid_kp"},
         {" Servo_turn_pid_ki"},
         {" Servo_turn_pid_kd"},
         {" Servo_mutu_pid_kp"},
         {" Servo_mutu_pid_ki"},
         {" Servo_mutu_pid_kd"},
         {"                  "}
        },
        {//�ٶ�
         {"Speed"},
         {" base_speed       "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "}
        },

        {//����
         {"Diffirence_Speed"},
         {" KLI_DEC          "},
         {" KLO_DEC          "},
         {" KRI_DEC          "},
         {" KRO_DEC          "},
         {" motor_pid_turn_kp"},
         {" motor_pid_turn_ki"},
         {" motor_pid_turn_kd"},
         {" motor_pid_mutu_kp"},
         {" motor_pid_mutu_ki"},
         {" motor_pid_mutu_kd"},
         {" k_dec            "},
        },
        {//�ؾ�
         {"Distance_control"},
         {" motor_pid_back_kp"},
         {" motor_pid_back_ki"},
         {" motor_pid_back_kd"},
         {" target_dist      "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "},
         {"                  "}
        },
        {//ͼ��
         {"Image"},
         {" light             "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "},
         {"                   "}
        },
};


void param_read(void)
{
        //**********�������**************
        UI_Param[0][0] = motor_pid_l.Kp;
        UI_Param[0][1] = motor_pid_l.Ki;
        UI_Param[0][2] = motor_pid_l.Kd;
        UI_Param[0][3] = motor_pid_r.Kp;
        UI_Param[0][4] = motor_pid_r.Ki;
        UI_Param[0][5] = motor_pid_r.Kd;

        //**********�������**************
        UI_Param[1][0] = Servo_dire_pid.kp;
        UI_Param[1][1] = Servo_dire_pid.ki;
        UI_Param[1][2] = Servo_dire_pid.kd;
        UI_Param[1][3] = Servo_turn_pid.kp;
        UI_Param[1][4] = Servo_turn_pid.ki;
        UI_Param[1][5] = Servo_turn_pid.kd;
        UI_Param[1][6] = Servo_mutu_pid.kp;
        UI_Param[1][7] = Servo_mutu_pid.ki;
        UI_Param[1][8] = Servo_mutu_pid.kd;

        //**********�ٶ�*****************
        UI_Param[2][0] = base_speed;

        //**********����*****************
        UI_Param[3][0] = KLI_DEC;
        UI_Param[3][1] = KLO_DEC;
        UI_Param[3][2] = KRI_DEC;
        UI_Param[3][3] = KRO_DEC;
        UI_Param[3][4] = motor_pid_turn.kp;
        UI_Param[3][5] = motor_pid_turn.ki;
        UI_Param[3][6] = motor_pid_turn.kd;
        UI_Param[3][7] = motor_pid_mutu.kp;
        UI_Param[3][8] = motor_pid_mutu.ki;
        UI_Param[3][9] = motor_pid_mutu.kd;
        UI_Param[3][10] = k_dec;

        //**********�ؾ�*****************
        UI_Param[4][0] = motor_pid_dist.kp;
        UI_Param[4][1] = motor_pid_dist.ki;
        UI_Param[4][2] = motor_pid_dist.kd;
        UI_Param[4][3] = target_dist;

        //**********ͼ��*****************
        UI_Param[5][0] = light;

        return;

}


void param_save(void)
{
        //**********�������**************
        motor_pid_l.Kp = UI_Param[0][0];
        motor_pid_l.Ki = UI_Param[0][1];
        motor_pid_l.Kd = UI_Param[0][2];
        motor_pid_r.Kp = UI_Param[0][3];
        motor_pid_r.Ki = UI_Param[0][4];
        motor_pid_r.Kd = UI_Param[0][5];

        //**********�������**************
        Servo_dire_pid.kp = UI_Param[1][0];
        Servo_dire_pid.ki = UI_Param[1][1];
        Servo_dire_pid.kd = UI_Param[1][2];
        Servo_turn_pid.kp = UI_Param[1][3];
        Servo_turn_pid.ki = UI_Param[1][4];
        Servo_turn_pid.kd = UI_Param[1][5];
        Servo_mutu_pid.kp = UI_Param[1][6];
        Servo_mutu_pid.ki = UI_Param[1][7];
        Servo_mutu_pid.kd = UI_Param[1][8];

        //**********�ٶ�*****************
        base_speed = UI_Param[2][0];

        //**********����*****************
        KLI_DEC = UI_Param[3][0];
        KLO_DEC = UI_Param[3][1];
        KRI_DEC = UI_Param[3][2];
        KRO_DEC = UI_Param[3][3];
        motor_pid_turn.kp = UI_Param[3][4];
        motor_pid_turn.ki = UI_Param[3][5];
        motor_pid_turn.kd = UI_Param[3][6];
        motor_pid_mutu.kp = UI_Param[3][7];
        motor_pid_mutu.ki = UI_Param[3][8];
        motor_pid_mutu.kd = UI_Param[3][9];
        k_dec = UI_Param[3][10];

        //**********�ؾ�*****************
        motor_pid_dist.kp = UI_Param[4][0];
        motor_pid_dist.ki = UI_Param[4][1];
        motor_pid_dist.kd = UI_Param[4][2];
        target_dist = UI_Param[4][3];
        //**********ͼ��*****************
        light = UI_Param[5][0];

        return;

}


void select_row(int row)
{

        static int last_row1 = 1;
        int row_height = 20;  // ÿ�м�࣬��ֹ����һ��
        int x_offset = 0;    // x ����ƫ�ƣ�����ͼ��λ��һ��

        // �����һ��ѡ����У���ʾ�հ�ͼ�꣩
        ips200_show_string(x_offset, (last_row1 + 1)* row_height, "  ");

        // �������ϻ���ѡ��ͼ��
        ips200_show_string(x_offset, (row + 1)* row_height,">>");

        // ��ʾ��ǰѡ�е�������ȷ�������ᱻͼ�긲��
        ips200_show_int(0, 0, row, 1);

        // ��¼��ǰ��
        last_row1 = row;
}

void select_row2(int row)
{

        static int last_row2 = 1;
        int y_offset  =  30;
        int row_height = 20;  // ÿ�м�࣬��ֹ����һ��
        int x_offset = 0;    // x ����ƫ�ƣ�����ͼ��λ��һ��

        // �����һ��ѡ����У���ʾ�հ�ͼ�꣩
        ips200_show_string(x_offset, last_row2 * row_height + y_offset, "  ");

        // �������ϻ���ѡ��ͼ��
        ips200_show_string(x_offset, row * row_height + y_offset, ">>");

        // ��ʾ��ǰѡ�е�������ȷ�������ᱻͼ�긲��
        ips200_show_int(0, 0, row, 1);

        // ��¼��ǰ��
        last_row2 = row;
}



/*****************************************************************
*Function: ��ʾ���˵�
*Description: ��ʾ���˵�����һ���˵������棬Ϊ�˹�����ʾЧ������Ҫ�趨��
*Description: ���˵��б����һ�п�ʼ��ʾ
*Return:
*****************************************************************/
void show_main_window(uint8 line)
{

       uint16 x = 20, y = 20;  // ��ʼ��ʾλ��

       /*for(int i = 0; i < sizeof(main_table)/sizeof(main_table[0]); i++)
       {
           ips200_show_string(x, y, main_table[i]);
           y += 20;  // ÿ�м�� 20 ���أ������ַ��ص�
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
*Function: ��ʾ���˵�
*Description: ��ʾ���˵����������˵�������
*parameter:  List ��һ�������˵���line �Ӷ����˵�����һ�п�ʼ��ʾ
*Return:
*****************************************************************/
void show_second(uint8_t List,uint8_t line)
{

        uint16 x = 20, y = 20;  // ������ʼ���꣬������ڿ������Ͻ�

        // ��ʾ������
        //ips200_show_int(0, 100, List,1);
        ips200_show_string(x, y, (char*)Second_table[List - 1][0]);
        y += 30;  // ������б�֮���������

        // ��ʾ�˵�����
        for (uint8 i = 0; i < 11; i++)
        {
            ips200_show_string(x, y, (char*)Second_table[List - 1][1 + i + line]);  // �ı�
            ips200_show_float(x + 150, y, UI_Param[List - 1][i + line], 4, 2);      // ��ֵ����
            y += 20;  // ÿ�м�� 20 ���أ���ֹ�ص�
        }
}

/*****************************************************************
*Function: ��ʾ��������
*Description: ��ʾ���ν���
*parameter:  List ��һ�������˵���line ��ʾ�ĸ�����
*Return:
*****************************************************************/
void show_variable(uint8_t List,uint8_t line)
{

     uint16 x = 10, y = 100;  // ������ʼ���꣬������ڿ������Ͻ�

     ips200_show_string(x, y, (char*)Second_table[List - 1][line]);  // �ı�
     ips200_show_float(x + 150, y, UI_Param[List - 1][line - 1], 4, 2);      // ��ֵ����

}

/*****************************************************************
*Function: ��ʾ����Ԥ�����
*Description: ��ʾ����Ԥ�����
*parameter:
*Return:
*****************************************************************/
void show_load(void)
{
     ips200_show_string(10, 20,"Select to load variables:");  // �ı�
     ips200_show_string(10, 60,"1 High speed");  // �ı�
     ips200_show_string(10, 160,"2 Low speed");  // �ı�
     //ips200_show_chinese(10, 20, 16, "��ѡ��Ҫ���ص�Ԥ�������", 20, RGB565_RED);
     //ips200_show_chinese(10, 60, 16, "1����", 5, RGB565_RED);
     //ips200_show_chinese(110, 60, 16, "2����", 5, RGB565_RED);
}

/*****************************************************************
*Function: һ���˵�����ʾ��ѡ����
*Description: ��ת��ʾ��ѡ�е���һ��
*parameter:
*Return:
*****************************************************************/
uint8_t cnt = 0;  // ��¼��ǰ������ʼ��
int8_t show_change(void)
{
    static int8_t LINE = 1;  // ��¼��ǰѡ����
    int8_t MAX_LINE = 9;     // �������������˵���� 9 �У�

    // ����������
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // ����
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < MAX_LINE)
    {
        LINE++;  // ����
    }
/*
    // ����˵���ҳ�߼�
    if (LINE - cnt >= 8)  // �����������·�ҳ
    {
        cnt++;
    }
    else if (LINE - cnt < 0)  // �����������Ϸ�ҳ
    {
        cnt--;
    }
*/
    // ����ѡ���е���ʾ
    show_main_window(cnt);
    select_row(LINE - cnt);

    key_clear_all_state();  // �������״̬
    key_scanner();  // ɨ�谴��

    return LINE;
}


/*****************************************************************
*Function: �����˵�����ʾ��ѡ����
*Description: ��ת��ʾ��ѡ�е���һ��
*parameter: line �ĸ������˵�
*Return:
*****************************************************************/
static uint8_t cnt2 = 0;  // ��¼�����˵�����ʼ��
uint8_t show_change2(uint8_t line)
{
    static uint8_t LINE_a = 1;       // ��¼��ǰѡ�е���
    static uint8_t line_last = 1;  // ��¼�ϴεĲ˵��������ظ���ʾͬһ�˵�

    // �����ǰ�˵����ϴβ˵���ͬ�����´�ͷ��ʼ��ʾ
    if (line_last != line)
    {
        LINE_a = 1;  // ����Ϊ��һ��
        cnt2 = 0;  // ���ò˵�ƫ��
    }
    line_last = line;

    // ����������
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE_a > 1)
    {
        LINE_a--;  // ����
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE_a < 11)  // �����������Ϊ9
    {
        LINE_a++;  // ����
    }
/*
    // ����˵���ҳ
    if (LINE_a - cnt2 >= 8)  // ��ǰ�г����˵���Χ�����·�ҳ
    {
        cnt2++;
    }
    else if (LINE_a - cnt2 < 0)  // ��ǰ�г����˵���Χ�����Ϸ�ҳ
    {
        cnt2--;
    }
*/
    // ����ѡ���е���ʾ
    show_second(line, cnt2);
    select_row2(LINE_a - cnt2);

    key_clear_all_state();  // �������״̬
    key_scanner();  // ɨ�谴��

    return LINE_a;  // ���ص�ǰѡ�е���
}

/*****************************************************************
*Function: ���κ���
*Description: ��ת��ʾ��ѡ�е���һ��
*parameter:line �ĸ������˵�,LINE �ĸ�ֵ, rate ��ʾ���ڲ���
*Return:
*****************************************************************/
void show_change3(uint8_t line, uint8_t LINE, float rate)
{

    if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
    {
        UI_Param[line - 1][LINE - 1] += rate;  // ����ֵ
    }
    else if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
    {
        UI_Param[line - 1][LINE - 1] -= rate;  // ����ֵ
    }



    // ��ʾ��ǰ���ڵĲ����Ͳ���ֵ

    ips200_show_float(90, 0, rate, 4, 3);
    show_variable(line,LINE);

    key_clear_all_state();  // �������״̬
    key_scanner();  // ɨ�谴��

    return;
}

/*****************************************************************
*Function: ��������ת��
*Description:
*parameter: cnt 1-5��������������λ
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
    int wave_flag = 0;          //״̬��
    uint8_t precision = 1;      //���ξ���
    float rate = 1;             //���β���
    bool loop_flag = true;      //ѭ����־λ
    int16_t LINE_1 = 1;         //һ���˵�ѡ�е���
    int16_t LINE_2 = 1;         //�����˵�ѡ�е���
    int i,j = 0;


    // ********* ��ʾ���� ******************
    /*
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);
    system_delay_ms(1000);
    ips200_clear();
    */
    ips200_show_string(0, 0, "UESTC");
    system_delay_ms(1000);

    param_read();

    // ��ʼ UI ����
    ips200_clear();
    show_main_window(0);
    select_row(1);



    while(loop_flag)
    {
        //***********����4�˳�ui********************
        if(key_get_state(KEY_4) == KEY_LONG_PRESS)
        {
            param_save();
            //UI_FLAG = false;
            loop_flag = false;
            break;
        }


        switch(wave_flag)
        {

            //**************һ���˵�**************************
            //***********2��3������ѡ��1��ȷ�ϣ�4��ȥԤ���������*****************
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


            //**************�����˵�**************************
            //***********2��3������ѡ��1��ȷ�ϣ�4���˳�ȥһ���˵�*****************
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



            //**************���ν���**************************
            //***********2��3���Ӽ�ֵ��1���Ĳ�����4���˳�ȥ�����˵�*****************
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


            //**************����Ԥ���������**************************
            //***********1��2���ֱ�ѡ���ײ�����4���˳�ȥһ���˵�*****************
            case 3 :
                key_clear_all_state();
                key_scanner();

                show_load();
                if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                {
                    for(i = 0; i < 11; i++)
                    {
                        for(j = 0; j < 11; j++)
                        {
                            UI_Param[i][j] = param_1[i][j];
                        }
                    }
                    param_save();
                    ips200_show_string(120, 60,"Load Success");  // �ı�
                    system_delay_ms(500);
                    ips200_show_string(120, 60,"            ");
                    //loop_flag = false;
                }
                else if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
                {
                    for(i = 0; i < 11; i++)
                    {
                        for(j = 0; j < 11; j++)
                        {
                            UI_Param[i][j] = param_2[i][j];
                        }
                    }
                    param_save();
                    ips200_show_string(120, 160,"Load Success");  // �ı�
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



