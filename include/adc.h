#ifndef ADC_H
#define ADC_H

#include "def.h"

#define SPI_CHANNEL 0
#define SPI_SPEED   1000000

int read_adc(int spi_handle, int channel); // ADC 값 읽기
int step_set(int adc_step); // 회생제동 단계 설정

#endif