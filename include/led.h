#ifndef LED_H
#define LED_H

#include "def.h"

#define LED_COUNT 8
#define LED_PIN 18   // GPIO18 (물리핀 12) - PWM
#define DMA 10
#define BRIGHTNESS 5
#define STRIP_TYPE WS2811_STRIP_GRB   // WS2812B는 일반적으로 GRB

#define COLOR(r, g, b) ( ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b) )


int led_INIT();
void led_set(uint32_t color);
void led_finish();

#endif