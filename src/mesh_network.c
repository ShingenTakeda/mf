#include <esp_log.h>
#include "mesh_network.h"

#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_RSSI -127
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN

#define NET_SSID "ValenteTakeda"
#define NET_PASSWD "ackmakVT96"
#define MESH_PASSWD "meterfarm"

QueueHandle_t mesh_send_q;
QueueHandle_t mesh_rcv_q;

bool is_mesh_connected = false;
int mesh_layer = -1;
esp_netif_t *netif_sta = NULL;

void ip_events(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(NET_TAG, "<IP_EVENT_STA_GET> IP:" IPSTR, IP2STR(&event->ip_info.ip));
}


bool is_mesh_parent_connected()
{
    return is_mesh_connected;
}

char *get_mesh_root_ip()
{
    if (netif_sta && esp_mesh_is_root())
    {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(netif_sta, &ip_info) == ESP_OK)
        {
            return ip4addr_ntoa((const ip4_addr_t *)&(ip_info.ip));
        }
    }
    return NULL;
}

void mesh_rcv_p2p_task()
{
    uint8_t data_buf[BUFF_SIZE];
    int flag = 0;

    mesh_addr_t sender;
    mesh_data_t m_data;

    m_data.data = data_buf;
    m_data.size = BUFF_SIZE;

    while(1)
    {
        if(esp_mesh_recv(&sender, &m_data, 1000 / portTICK_PERIOD_MS, &flag, NULL, 0) == ESP_OK)
        {
            
        }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void mesh_send_p2p_task()
{
    
} 

void wifi_events(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch(event_id)
    {
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(NET_TAG, "<WIFI_EVENT_STA_CONNECTED>");
        break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(NET_TAG, "<WIFI_EVENT_STA_DISCONNECTED>");
        break;
    }
}

void mesh_init()
{
    ESP_ERROR_CHECK(esp_mesh_init());
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_events, NULL));

    mesh_cfg_t config = MESH_INIT_CONFIG_DEFAULT();
    memcpy((uint8_t *) &config.mesh_id, MAC_ADDR, 6);

    config.channel = 0;
    config.router.ssid_len = strlen(NET_SSID);
    memcpy((uint8_t *) &config.router.ssid, NET_SSID, config.router.ssid_len);
    memcpy((uint8_t *) &config.router.password, NET_PASSWD, strlen(NET_PASSWD));

    config.mesh_ap.max_connection = 2;
    config.mesh_ap.nonmesh_max_connection = 1;

    memcpy((uint8_t *)&config.mesh_ap.password, MESH_PASSWD, strlen(MESH_PASSWD));

    ESP_ERROR_CHECK(esp_mesh_set_config(&config));
    ESP_ERROR_CHECK(esp_mesh_start());
    ESP_ERROR_CHECK(esp_mesh_set_self_organized(true, true));
}

void wifi_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_events, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_events, NULL, NULL));

    // Initialize default station as network interface instance (esp-netif)
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Initialize and start WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = NET_SSID,
            .password = NET_PASSWD,
            .scan_method = DEFAULT_SCAN_METHOD,
            .sort_method = DEFAULT_SORT_METHOD,
            .threshold = {
                .rssi = DEFAULT_RSSI,
                .authmode = DEFAULT_AUTHMODE,
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void mesh_system_init()
{
    wifi_init();
    mesh_init();
}