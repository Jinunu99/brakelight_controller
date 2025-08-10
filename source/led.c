#include "../include/led.h"

ws2811_t ledstring = {
    .freq = WS2811_TARGET_FREQ,
    .dmanum = DMA,
    .channel = {
        [0] = {
            .gpionum = LED_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = BRIGHTNESS,
            .strip_type = STRIP_TYPE
        },
        [1] = {0}
    }
};

int led_INIT()
{
    if (ws2811_init(&ledstring) != WS2811_SUCCESS)
    {
        return -1; // led 초기화 실패
    }

    led_set(COLOR(0, 0, 0)); // LED OFF
    return 0;
}

void led_set(uint32_t color)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        ledstring.channel[0].leds[i] = color;
    }
    ws2811_render(&ledstring);
}

void led_finish()
{
    led_set(COLOR(0, 0, 0)); // LED OFF
    ws2811_fini(&ledstring);
}