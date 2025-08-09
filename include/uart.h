#ifndef UART_H
#define UART_H

#include "def.h"

#define SERIAL_PORT "/dev/serial0"

int uart_open(const char *device);

#endif