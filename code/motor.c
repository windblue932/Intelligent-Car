/*
 * motor.c
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */

#include "zf_common_headfile.h"


float Inc_L;
float Inc_R;
float difference_speed;
volatile int count = 0;
volatile int mid_count = 0;
int motor_count = 0;
motor1 motor_l;
motor1 motor_r;


void Encoder_Init(void)
{
    //左编码器
    encoder_dir_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);
    //右编码器
    encoder_dir_init(TIM6_ENCODER,TIM6_ENCODER_CH1_P20_3,TIM6_ENCODER_CH2_P20_0);

}


void Encoder_Data_Get(void)
{
    float distance_per_pulse = 3.14159f * 0.065f / 2322.0f; // m/pulse
    float sampling_rate = 200.0f; // 1 / 0.005s

    motor_l.encoder_raw = -encoder_get_count(TIM4_ENCODER);
    motor_l.encoder_speed = motor_l.encoder_raw;
    motor_l.real_speed = motor_l.encoder_speed * distance_per_pulse * sampling_rate;
    motor_l.total_encoder += motor_l.encoder_raw;
    encoder_clear_count(TIM4_ENCODER);

    motor_r.encoder_raw = encoder_get_count(TIM6_ENCODER);
    motor_r.encoder_speed = motor_r.encoder_raw;
    motor_r.real_speed = motor_r.encoder_speed * distance_per_pulse * sampling_rate;
    motor_r.total_encoder += motor_r.encoder_raw;
    encoder_clear_count(TIM6_ENCODER);
}


//电机初始化
void MotorInit(unsigned short freq)
{
    gpio_init(Motor_GPIO_Left, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(Motor_GPIO_Right, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    pwm_init(Motor_PWM_Left, freq, 0);
    pwm_init(Motor_PWM_Right, freq, 0);

}


//设置单独电机
void Speed_Set(pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus)
{
    if(speed>0)
    {
        pwm_set_duty(pin1, speed);
        gpio_set_level(pin2, plus);
    }
    else
    {
        pwm_set_duty(pin1, -speed);
        gpio_set_level(pin2, minus);
    }

}


//电机速度闭环控制
void Motor_Control(int Speed_L, int Speed_R)
{
    motor_l.target_speed=Speed_L;
    motor_r.target_speed=Speed_R;

    Inc_L = PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed);
    Inc_R = PID_Increase(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed);


    motor_l.duty=limit_int(-pid_down_left_limit, motor_l.duty + Inc_L, pid_up_left_limit);
    motor_r.duty=limit_int(-pid_down_right_limit, motor_r.duty + Inc_R, pid_up_right_limit);

    Speed_Set(Motor_PWM_Left, Motor_GPIO_Left, motor_l.duty, 0, 1);
    Speed_Set(Motor_PWM_Right, Motor_GPIO_Right, motor_r.duty, 0, 1);
}


void Motor_ControlR(int Speed_R)
{
    motor_r.target_speed=Speed_R;

    motor_r.duty=limit_int(-pid_out_limit,motor_r.duty+PID_Increase(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed),pid_out_limit);

    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,motor_r.duty,0,1);
}


void Motor_ControlL(int Speed_L){

    motor_l.target_speed=Speed_L;

    motor_l.duty=limit_int(-pid_out_limit,motor_l.duty+PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);

    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
}


void Final_Motor_Control(int speed,float mid)
{
    if(speed != 0){

        if(Straight_Flag)
        {
            difference_speed = PidLocCtrltest(&servo_pid_dire,mid);
        }
        else if(Mutation_Flag || Left_Island_state || Right_Island_state)
        {
            difference_speed = PidLocCtrltest(&servo_pid_muta,mid);
        }
        else
        {
            difference_speed = PidLocCtrltest(&servo_pid_turn,mid);
        }


        //difference_speed = PidLocCtrltest(&servo_pid_turn,mid);


        if(difference_speed <= 0)
            Motor_Control(speed + KLI_DEC * difference_speed,speed - KLO_DEC * difference_speed);
        else
            Motor_Control(speed + KRO_DEC * difference_speed,speed - KRI_DEC * difference_speed);

    }
    else{
        Motor_Control(0,0);
    }

}

void Run_Car(void){

    //遥控**P20_11 = A，启动电机；P20_12 = B，停止电机；P20_13 = C，待定（负压风扇）
    boolean p20_11 = IfxPort_getPinState(&MODULE_P20, 11);
    boolean p20_12 = IfxPort_getPinState(&MODULE_P20, 12);
    boolean p20_13 = IfxPort_getPinState(&MODULE_P20, 13);
    if(!(Flag.Garage_Find && (isr_count >= start_time + 100))){//状态决策
        if(!p20_11 && p20_12){//A
            Flag.GOGOGO = true;
        }
        else if(p20_11 && !p20_12){//B
            Flag.GOGOGO = false;
        }
        else{
            ;
        }
    }
    else{
        Flag.GOGOGO = false;
    }
    if(Flag.GOGOGO){//驱动实行
        if(!motor_count)
        {
            motor_count = isr_count;
        }
        if(isr_count > motor_count + 450)
        {
            Final_Motor_Control(Purpost_Speed,servo_param.Servo_filter);
        }
        else
        {
            Motor_Control(0,0);
        }
        Flag.Fan = true;
    }
    else{
        Motor_Control(0,0);
        Flag.Fan = false;
    }

}

void motor_creazy_protect(void){
        if(my_abs(motor_l.duty) >= (pid_out_limit - 100) || my_abs(motor_r.duty) >= (pid_out_limit - 100))
            creazy_count++;
        else
            creazy_count = 0;
        if(creazy_count >= 3000)
            Flag.Motor_Creazy = true;
}

void motor_stem_protect(void){
    if(!Flag.GOGOGO)
        Flag.Motor_Stem = false;
    else{
        if((motor_l.target_speed - motor_l.encoder_speed) >= 250 || (motor_r.target_speed - motor_r.encoder_speed) >= 250)
            stem_count++;
        else
            stem_count = 0;
        if(stem_count >= 200)
            Flag.Motor_Stem = true;
    }
}

//110      1250
//55        750
//2322    一圈脉冲数
