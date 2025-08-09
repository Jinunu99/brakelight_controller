#include "../include/adc.h"

// ADC (MCP3008)
int read_adc(int spi_handle, int channel)
{
    char txBuf[3], rxBuf[3];
    txBuf[0] = 0x01;
    txBuf[1] = 0x80 | (channel << 4);
    txBuf[2] = 0x00;
    spiXfer(spi_handle, txBuf, rxBuf, 3);
    return ((rxBuf[1] & 0x03) << 8) | (rxBuf[2] & 0xFF);
}

// 회생제동 단계 설정
int step_set(int adc_step)
{
    if (adc_step < 341) return 1;
    else if (adc_step < 682) return 2;
    else return 3;
}