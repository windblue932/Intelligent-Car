/*
 * motor.h
 *
 *  Created on: 2023年5月8日
 *      Author: Kern
 */
#include "zf_common_headfile.h"

#ifndef MOTORSERVO_H_
#define MOTORSERVO_H_

#define Servo_PWM_PIN                   (ATOM1_CH1_P33_9)// TIM5_PWM_MAP0_CH3_A2
#define Servo_Delta                     (155)                             //舵机左右转动的差值，与舵机型号，拉杆和舵机臂长有关
#define Servo_Center_Mid                (1185)                             //舵机直行中值，585
#define Servo_Left_Max                  (Servo_Center_Mid+Servo_Delta)      //舵机左转极限值
#define Servo_Right_Min                 (Servo_Center_Mid-Servo_Delta)      //舵机右转极限值，此值跟舵机放置方式有关，立式

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
}motor1;

extern struct motor1 motor_l;
extern struct motor1 motor_r;

extern float Inc_L;
extern float Inc_R;
extern int16_t count_stop_near;
extern int16_t count_stop_loss;
extern int creazy_count;
extern int stem_count;
extern float deltaspeed;
extern float target_speed;
extern float difference_speed;

void ServoInit                          (unsigned short freq);
void ServoCtrl                          (unsigned short int duty);
void Encoder_Init                       (void);
void Encoder_Data_Get                   (void);
void MotorInit                          (unsigned short freq);
void Speed_Set                          (pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus);
void Motor_Control                      (int Speed_L, int Speed_R);
void Motor_ControlL                     (int Speed_L);
void Motor_ControlR                     (int Speed_R);
void shut_down                          (void);
void motor_creazy_protect               (void);
void motor_stem_protect                 (void);
void backspeedcontrol                   (float delta,float delta_target,float base_speed,float k1,float motorerror);

#endif /* MOTORSERVO_H_ */
