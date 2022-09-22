#ifndef BLink_h
#define BLink_h

#include <Arduino.h>

void blink(int times, int dl)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    bool led_on = false;
    for (int i = 0; i <= times * 2; i++)
    {
        delay(dl);
        led_on = !led_on;
        digitalWrite(LED_BUILTIN, led_on ? LOW : HIGH);
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

void blink(int times)
{
    blink(times, 300);
}

void blink()
{
    blink(100, 300);
}

void blink_err()
{
    blink(50, 150);
}

void blink_ok()
{
    blink(3, 300);
}

#endif