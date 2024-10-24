#pragma once

#define CLIENT_PORT 8888
#define SERVER_PORT 9999

void net_system_init();
void net_system_deinit();
void net_wifi_ip_address_clear();
void net_wifi_ip_addrees_save(esp_ip4_addr_t ip);