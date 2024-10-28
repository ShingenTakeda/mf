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
        
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void net_server_task(void *args)
{
    char rx_buffer[BUFF_SIZE];
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;
    int PORT = CLIENT_PORT;

    while (1)
    {

        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;

        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(NET_TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(NET_TAG, "Socket created");

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(NET_TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(NET_TAG, "Socket bound, port %d", PORT);

        struct sockaddr_storage source_addr;
        socklen_t socklen = sizeof(source_addr);

        while (1)
        {
            ESP_LOGI(NET_TAG, "Waiting for data");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            // Error occurred during receiving
            if (len < 0)
            {
                ESP_LOGI(NET_TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else
            {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET)
                {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                }
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(NET_TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(NET_TAG, "%s", rx_buffer);

                // Sending back the same message to acknowledge delivery
                int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                if (err < 0)
                {
                    ESP_LOGE(NET_TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1)
        {
            ESP_LOGI(NET_TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void net_system_init()
{
    if(net_server_t_handle == NULL)
    {
        xTaskCreatePinnedToCore(net_server_task, "UDP SERVER TASK", 4096, NULL, 3, net_server_t_handle, 0);
    }
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