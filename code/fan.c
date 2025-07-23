/*
 * fan.c
 *
 *  Created on: 2025Äê6ÔÂ12ÈÕ
 *      Author: 21359
 */
#include "zf_common_headfile.h"
uint16_t begin_pwm = 0;
uint16_t fan_count = 0;

void fan_init(unsigned short freq){

    pwm_init(FAN_PWM_LEFT, freq, 0);
    pwm_init(FAN_PWM_RIGT, freq, 0);
}

/*
void fan_work(int pwm){
    if(begin_pwm < pwm){
        fan_count ++;
        if(fan_count >= 2)
        {
            begin_pwm += 10;
            fan_count = 0;
        }
    }
    else begin_pwm = constrain_int32(pwm, 0, 1000);
    if(Flag.GOGOGO){

        pwm_set_duty(FAN_PWM_LEFT, begin_pwm);
        pwm_set_duty(FAN_PWM_RIGT, begin_pwm);
    }else{
    pwm_set_duty(FAN_PWM_LEFT, 0);
    pwm_set_duty(FAN_PWM_RIGT, 0);
    }
}
*/

void fan_work(int pwm){
    pwm = constrain_int32(pwm, 0, 1000);
    if(Flag.GOGOGO){
        if(!fan_count){
            fan_count = isr_count;
        }
        if((isr_count >= fan_count) && (isr_count <= (fan_count + 100))){
        pwm_set_duty(FAN_PWM_LEFT, 600);
        pwm_set_duty(FAN_PWM_RIGT, 600);
        }
        else if(isr_count > (fan_count + 100) && isr_count <= (fan_count + 200)){
            pwm_set_duty(FAN_PWM_LEFT, 600);
            pwm_set_duty(FAN_PWM_RIGT, 600);
        }
        else if(isr_count > (fan_count + 200) && isr_count <= (fan_count + 300)){
            pwm_set_duty(FAN_PWM_LEFT, 700);
            pwm_set_duty(FAN_PWM_RIGT, 700);
        }
        else if(isr_count > (fan_count + 300) && isr_count <= (fan_count + 400)){
            pwm_set_duty(FAN_PWM_LEFT, 700);
            pwm_set_duty(FAN_PWM_RIGT, 700);
        }
        else{
            pwm_set_duty(FAN_PWM_LEFT, pwm);
            pwm_set_duty(FAN_PWM_RIGT, pwm);
        }
    }
    else{
        pwm_set_duty(FAN_PWM_LEFT, 0);
        pwm_set_duty(FAN_PWM_RIGT, 0);
    }
}
