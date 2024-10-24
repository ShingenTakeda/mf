#include <stdint.h>

#include <esp_log.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

#include "common.h"
#include "net_tasks.h"

//TODO:Change this after getting a static ip for the server
#define SERVER_IP "192.168.0.174"

TaskHandle_t net_client_t_handle;
TaskHandle_t net_server_t_handle;

mpack_reader_t reader;

char *wifi_ip_address = NULL;
char *wifi_ip_broadcast = NULL;

void net_wifi_ip_address_clear()
{
    if(wifi_ip_address != NULL) 
    {
        free(wifi_ip_address);
        wifi_ip_address = NULL;
    }
    if(wifi_ip_broadcast != NULL) 
    {
        free(wifi_ip_broadcast);
        wifi_ip_broadcast = NULL;
    }
}

void net_wifi_ip_address_save(esp_ip4_addr_t ip)
{
    int ipSize = 16;
    wifi_ip_address = malloc(ipSize);
    wifi_ip_broadcast = malloc(ipSize);
    snprintf(wifi_ip_address, ipSize, IPSTR, IP2STR(&ip));
    snprintf(wifi_ip_broadcast, ipSize, IPSTR, esp_ip4_addr1_16(&ip), esp_ip4_addr2_16(&ip), esp_ip4_addr3_16(&ip), 255);
}

void net_client_task(void *args)
{
    uint8_t buffer[BUFF_SIZE];
    char * rcv_buffer;
    int flag = 0;

    mesh_addr_t sender;
    mesh_data_t data;

    struct sockaddr_in dest_addr = 
    {
        .sin_addr.s_addr = inet_addr(SERVER_IP),
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    while(1)
    {
        /*
        if(esp_mesh_recv(&sender, &data, portMAX_DELAY, &flag, NULL, 0) == ESP_OK)
        {
            asprintf(&tx_buf, "remetente: "MACSTR", msg: %s\n", MAC2STR(sender.addr), data.data); // prepara buffer para envio
            ESP_LOGI(MESH_TAG, "%s", tx_buf); // imprime buffer nos logs
            sendto(sock, tx_buf, strlen(tx_buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)); // enviar buffer para servidor remoto
            free(tx_buf);
        }
        */

    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void net_server_task(void *args)
{
    uint8_t buffer[BUFF_SIZE] = {0};
    mesh_data_t data;
    data.data = buffer;
    data.size = BUFF_SIZE;

    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);

    struct sockaddr_in dest_addr = 
    {
        .sin_addr.s_addr = inet_addr("0.0.0.0"),
        .sin_family = AF_INET,
        .sin_port = htons(CLIENT_PORT)
    };

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    while(1)
    {
        recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *) &source_addr, &socklen);
        ESP_LOGI(MESH_TAG, "%s", buffer);

        //Clear buffer
        memset(buffer, 0, sizeof(buffer));
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void net_system_init()
{
}

void net_system_deinit()
{
    if(net_client_t_handle != NULL)
    {
        vTaskDelete(net_client_t_handle);
    }
    
    if(net_server_t_handle != NULL)
    {
        vTaskDelete(net_server_t_handle);
    }
}