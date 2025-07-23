/*
 * send.c
 *
 *  Created on: 2025年3月27日
 *      Author: yolo
 */

#include "zf_common_headfile.h"

#define WIFI_SSID_TEST          "qgyd"
#define WIFI_PASSWORD_TEST      "12345678"  // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL

void wireless_send_int(int number){
    //无线串口发送整型数据
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%d", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_double(double number){
    //无线串口发送浮点数据
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%.3f", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_string(char number){
    //无线串口发送字符数据
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
    //wifi spi 连接
    int i = 0;
    //uint8 image_copy[MT9V03X_H][MT9V03X_W];
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        i++;
        if (i > 100)
            break;
        printf("\r\n connect wifi failed. \r\n");
        system_delay_ms(100);                                                   // 初始化失败 等待 100ms
    }

    printf("\r\n module version:%s",wifi_spi_version);                          // 模块固件版本
    printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // 模块 MAC 信息
    printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // 模块 IP 地址

    // zf_device_wifi_spi.h 文件内的宏定义可以更改模块连接(建立) WIFI 之后，是否自动连接 TCP 服务器、创建 UDP 连接
    int j = 0;
    if(1 != WIFI_SPI_AUTO_CONNECT)                                              // 如果没有开启自动连接 就需要手动连接目标 IP
    {

        while(wifi_spi_socket_connect(                                          // 向指定目标 IP 的端口建立 TCP 连接
            "TCP",                                                              // 指定使用TCP方式通讯
            WIFI_SPI_TARGET_IP,                                                 // 指定远端的IP地址，填写上位机的IP地址
            WIFI_SPI_TARGET_PORT,                                               // 指定远端的端口号，填写上位机的端口号，通常上位机默认是8080
            WIFI_SPI_LOCAL_PORT))                                               // 指定本机的端口号
        {
            j++;
            if (j > 100)
                break;
            // 如果一直建立失败 考虑一下是不是没有接硬件复位
            printf("\r\n Connect TCP Servers error, try again.");
            system_delay_ms(100);                                               // 建立连接失败 等待 100ms
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
    //wifi spi 发送
    if(spi_connect == true)
    {
        memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
        seekfree_assistant_camera_send();
    }
}

