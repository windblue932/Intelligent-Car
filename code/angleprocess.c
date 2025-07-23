#include "zf_common_headfile.h"

signed short pwm_output;
float output = 0;
uint8_t add_speed;
void turn_Update(void)
{

    if(servo_error <= 18 && servo_error >= -18){
        output = PidLocCtrl(&Servo_dire_pid, servo_error);  // PID输出
        difference_speed = PidLocCtrl(&motor_pid_turn,motorerror);
        add_speed = 0;
    }
    else if(servo_error > 30 || servo_error < -30){
        output = PidLocCtrl(&Servo_mutu_pid, servo_error);  // PID输出
        difference_speed = PidLocCtrl(&motor_pid_mutu,motorerror);
        add_speed = 3;
    }
    else{
        output = PidLocCtrl(&Servo_turn_pid, servo_error);
        difference_speed = PidLocCtrl(&motor_pid_mutu,motorerror);
        add_speed = 3;
    }

    pwm_output = constrain_float(Servo_Center_Mid + output, Servo_Right_Min, Servo_Left_Max - 5);    //PWM增大左拐   限幅待确认



    pwm_set_duty(Servo_PWM_PIN, pwm_output);
}




