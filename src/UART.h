#pragma once
#include "common.h"

//TODO Mock how to set UART comms(protocol and hardware)
void UART_init();

//TODO Mock ABNT UART here
void UART_task(void *args);