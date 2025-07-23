/*
 * send.c
 *
 *  Created on: 2025��3��27��
 *      Author: yolo
 */

#include "zf_common_headfile.h"

#define WIFI_SSID_TEST          "qgyd"
#define WIFI_PASSWORD_TEST      "12345678"  // �����Ҫ���ӵ�WIFI û����������Ҫ�� ���� �滻Ϊ NULL

void wireless_send_int(int number){
    //���ߴ��ڷ�����������
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%d", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_double(double number){
    //���ߴ��ڷ��͸�������
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%.3f", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_string(char number){
    //���ߴ��ڷ����ַ�����
    char byte[2];
    byte[0] = number;
    byte[1] = '\0';

    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string(byte);
}

uint16_t const image_copy[MT9V03X_H][MT9V03X_W] = {0};
bool spi_connect = false;
void wifi_spi_connect(void){
    //wifi spi ����
    int i = 0;
    //uint8 image_copy[MT9V03X_H][MT9V03X_W];
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        i++;
        if (i > 100)
            break;
        printf("\r\n connect wifi failed. \r\n");
        system_delay_ms(100);                                                   // ��ʼ��ʧ�� �ȴ� 100ms
    }

    printf("\r\n module version:%s",wifi_spi_version);                          // ģ��̼��汾
    printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // ģ�� MAC ��Ϣ
    printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // ģ�� IP ��ַ

    // zf_device_wifi_spi.h �ļ��ڵĺ궨����Ը���ģ������(����) WIFI ֮���Ƿ��Զ����� TCP ������������ UDP ����
    int j = 0;
    if(1 != WIFI_SPI_AUTO_CONNECT)                                              // ���û�п����Զ����� ����Ҫ�ֶ�����Ŀ�� IP
    {

        while(wifi_spi_socket_connect(                                          // ��ָ��Ŀ�� IP �Ķ˿ڽ��� TCP ����
            "TCP",                                                              // ָ��ʹ��TCP��ʽͨѶ
            WIFI_SPI_TARGET_IP,                                                 // ָ��Զ�˵�IP��ַ����д��λ����IP��ַ
            WIFI_SPI_TARGET_PORT,                                               // ָ��Զ�˵Ķ˿ںţ���д��λ���Ķ˿ںţ�ͨ����λ��Ĭ����8080
            WIFI_SPI_LOCAL_PORT))                                               // ָ�������Ķ˿ں�
        {
            j++;
            if (j > 100)
                break;
            // ���һֱ����ʧ�� ����һ���ǲ���û�н�Ӳ����λ
            printf("\r\n Connect TCP Servers error, try again.");
            system_delay_ms(100);                                               // ��������ʧ�� �ȴ� 100ms
        }
    }
    if(i < 100 && j < 100)
    {
        spi_connect = true;
        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], MT9V03X_W, MT9V03X_H);
    }

}

void wifi_spi_send(void){
    //wifi spi ����
    if(spi_connect == true)
    {
        memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
        seekfree_assistant_camera_send();
    }
}

