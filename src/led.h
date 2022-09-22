#ifndef LED_h
#define LED_h

#include <Arduino.h>

const int LED_PINS[] = {0, 4, 10, 14, 16};
const int LED_PINS_LEN = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

int led_bri = 0;       //[0, 100]

void led_update_v(int v)
{
    for (int i = 0; i < LED_PINS_LEN; i++)
    {
        if (i * 20 < v)
        {
            printf("序号%d ON\n", i);
            digitalWrite(LED_PINS[i], HIGH);
        }
        else
        {
            printf("序号%d OFF\n", i);
            digitalWrite(LED_PINS[i], LOW);
        }
    }
    led_bri = v;
}

int get_light_bri()
{
    return led_bri;
}

#endif