/*
 * fan.h
 *
 *  Created on: 2025Äê6ÔÂ12ÈÕ
 *      Author: 21359
 */

#ifndef CODE_FAN_H_
#define CODE_FAN_H_

#define FAN_PWM_LEFT                     (ATOM2_CH0_P33_4)
#define FAN_PWM_RIGT                     (ATOM2_CH1_P33_5)

void fan_init                         (unsigned short freq);
void fan_work                         (int pwm);
extern uint16_t begin_pwm;
extern uint16_t fan_count;


#endif /* CODE_FAN_H_ */
