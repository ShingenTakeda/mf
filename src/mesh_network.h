#pragma once
#include "common.h"


//TODO Get a static IP for root esp
void mesh_events(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data);
void mesh_system_init();