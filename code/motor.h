/*
 * motor.h
 *
 *  Created on: 2025��3��10��
 *      Author: 15208
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"



#define Motor_PWM_Left                  (ATOM0_CH3_P21_5)// TIM8_PWM_MAP0_CH1_C6
#define Motor_PWM_Right                 (ATOM0_CH1_P21_3)// TIM8_PWM_MAP0_CH2_C7
#define Motor_GPIO_Left                 (P21_4 )// E4
#define Motor_GPIO_Right                (P21_2 )// E3


typedef struct motor1
{
        int target_speed;
        int duty;
        int encoder_speed;
        int encoder_raw;
        int32 total_encoder;
        float real_speed;
}motor1;


extern struct motor1 motor_l;
extern struct motor1 motor_r;

extern float difference_speed;
extern float Inc_L;
extern float Inc_R;
extern int motor_count;

void Encoder_Init(void);
void Encoder_Data_Get(void);
void MotorInit(unsigned short freq);
void Speed_Set(pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus);
void Motor_Control(int Speed_L, int Speed_R);
void Motor_ControlR(int Speed_R);
void Motor_ControlL(int Speed_L);
void Final_Motor_Control(int speed,float mid);
void Run_Car(void);
void motor_creazy_protect(void);
void motor_stem_protect(void);

#endif /* CODE_MOTOR_H_ */
