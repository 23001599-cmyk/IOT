#include <Arduino.h>
#include "LED.h"
#include <OneButton.h>

LED led(LED_PIN, LED_ACT);

void btnClick();
void btnDoubleClick();
OneButton button(BTN_PIN, !BTN_ACT);

void setup()
{
    led.off();
    button.attachClick(btnClick);              // 1 lan bam  -> bat/tat LED
    button.attachDoubleClick(btnDoubleClick);  // bam 2 lan  -> nhap nhay LED
}

void loop()
{
    led.loop();
    button.tick();
}

void btnClick()
{
    led.flip();
}

void btnDoubleClick()
{
    led.blink(200);
}
