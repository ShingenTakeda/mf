#pragma once
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_mesh.h>
#include <esp_mesh_internal.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <esp_event.h>
#include <driver/uart.h>
#include <esp_system.h>

#include "mpack.h"

#define NET_TAG "NET"
#define MESH_TAG "MESH"

#define BUFF_SIZE 1024

#define MAC_ADDR (uint8_t[]){0x77, 0x77, 0x77, 0x77, 0x77, 0x77}

typedef enum
{
    ABNT,
    DLMS,
    OU_NORMAL,
    OU_EXTENDED,
    OU_MIXED,
    PIMA,    
}METER_PROTOCOL_TYPE;

typedef enum
{
    _232,
    _485,
    ERROR
}UART_CON_TYPE;

typedef struct
{
    uint8_t *str;
    uint32_t size;    
}string_t;

//ABNT serial size: 4 bytes
//DLMS serial size: 8-12 bytes
typedef struct 
{
    bool is_root;
    METER_PROTOCOL_TYPE protocol_type;
    UART_CON_TYPE uart_type;
    uint8_t mac_addr[6];
    string_t serial_num;
}meter_attrib_t;

extern mpack_reader_t reader;
extern meter_attrib_t meter_attrib;

extern mesh_addr_t mesh_parent_addr;
extern bool is_mesh_connected;
extern int mesh_layer;
extern esp_netif_t *netif_sta;

extern uint8_t MY_MAC[6];

extern QueueHandle_t net_client_q;

extern QueueHandle_t mesh_send_q;
extern QueueHandle_t mesh_rcv_q;

extern QueueHandle_t uart_q;

extern TaskHandle_t mesh_rcv_t_handle;
extern TaskHandle_t mesh_send_t_handle;

extern TaskHandle_t net_client_t_handle;
extern TaskHandle_t net_server_t_handle;