/*
 * beep.h
 *
 *  Created on: 2023��5��8��
 *      Author: Kern
 */

#ifndef BEEP_H_
#define BEEP_H_

#include "zf_common_headfile.h"

#define   Beep_PWM_Pin              (ATOM3_CH0_P33_10)

typedef enum
{
    BEEP_OFF,
    BEEP_ON,
}beep_state_enum;


void beep_init                              (void);
void beep_set_sound                         (beep_state_enum state);
void beep_set_freq                          (int freq);
void beep_sound_point                       (void);

#endif /* BEEP_H_ */

