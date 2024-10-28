#pragma once
#include <stdint.h>
#include "common.h"

//util functions
void meter_attrib_to_mpack(meter_attrib_t *meter_attributes);

//util tasks
void send_meter_attrib_task(void *args);

//mesh util tasks
void check_mesh_health(void *args);