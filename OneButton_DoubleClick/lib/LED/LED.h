/***
 * LED.h: A class for Led objects
 * By: Nguyen Anh Tuan (Apr., 2022)
 * 
 ***/
#pragma once
#include <Arduino.h>

class LED
{
public:
    LED(const byte pin, const bool active);
    void on();
    void off();
    void flip();
    void blink(int duration);
    void loop();

private:
    const byte _pin;
    const bool _active; // LOW/HIGH

    unsigned long _timer = 0;   // ms
    unsigned int _duration = 0; // ms

    enum States
    {
        OFF,
        ON,
        BLINK,
    } state = OFF;
};

/*** Implementation: ***/

LED::LED(const byte pin, const bool active) : _pin(pin), _active(active)
{
    pinMode(_pin, OUTPUT);
    off();
}

void LED::on()
{
    digitalWrite(_pin, _active);
    state = ON;
}

void LED::off()
{
    digitalWrite(_pin, !_active);
    state = OFF;
}

void LED::flip()
{
    if (state == OFF)
        on();
    else
        off();
}

void LED::blink(int duration)
{
    _duration = duration;
    state = BLINK;
}

void LED::loop()
{
    switch (state)
    {
    case OFF:
    case ON:
        break;

    case BLINK:
        if ((millis() - _timer) > _duration)
        {
            digitalWrite(_pin, !digitalRead(_pin));
            _timer = millis();
        }
        break;
    }
}
