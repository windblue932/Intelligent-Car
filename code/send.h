/*
 * send.h
 *
 *  Created on: 2025Äê3ÔÂ27ÈÕ
 *      Author: yolo
 */

#ifndef CODE_SEND_H_
#define CODE_SEND_H_


#include "zf_common_headfile.h"

void wireless_send_int(int number);
void wireless_send_double(double number);
void wireless_send_string(char number);
void wireless_send_imag();
void wifi_spi_connect(void);
void wifi_spi_send(void);
extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
extern bool spi_connect;


#endif /* CODE_SEND_H_ */
