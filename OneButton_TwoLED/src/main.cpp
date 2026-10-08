/***
 * Dieu khien hai LED bang mot nut bam (thu vien OneButton + LED.h)
 *
 *   single click : bat/tat LED dang duoc chon
 *   double click : chuyen LED dang duoc chon (LED1 <-> LED2)
 *   giu nut      : LED dang duoc chon nhap nhay, 200 ms doi trang thai mot lan
 *
 * Chan GPIO va muc tich cuc duoc khai bao trong platformio.ini (build_flags):
 *   LED1_PIN, LED1_ACT, LED2_PIN, LED2_ACT, BTN_PIN, BTN_ACT
 ***/
#include <Arduino.h>
#include "LED.h"
#include <OneButton.h>

#ifndef HOLD_MS
#define HOLD_MS 2000 // thoi gian giu nut de coi la "giu" (ms)
#endif
#define BLINK_MS 200 // chu ky nhap nhay (ms)

LED led1(LED1_PIN, LED1_ACT);
LED led2(LED2_PIN, LED2_ACT);

LED *const leds[] = {&led1, &led2};
const uint8_t LED_COUNT = sizeof(leds) / sizeof(leds[0]);
uint8_t selected = 0; // chi so LED dang duoc dieu khien: 0 = LED1, 1 = LED2

OneButton button(BTN_PIN, !BTN_ACT);

void btnClick();
void btnDoubleClick();
void btnHold();

void setup()
{
    Serial.begin(115200);

    led1.off();
    led2.off();

    button.setPressMs(HOLD_MS);
    button.attachClick(btnClick);
    button.attachDoubleClick(btnDoubleClick);
    button.attachLongPressStart(btnHold);

    Serial.println("Dang dieu khien: LED1");
}

void loop()
{
    led1.loop();
    led2.loop();
    button.tick();
}

// 1 lan bam: bat/tat LED dang chon (neu dang nhap nhay thi tat)
void btnClick()
{
    leds[selected]->flip();
    Serial.printf("LED%u: flip\n", selected + 1);
}

// 2 lan bam: chuyen sang LED con lai. LED cu giu nguyen trang thai
void btnDoubleClick()
{
    selected = (selected + 1) % LED_COUNT;
    Serial.printf("Dang dieu khien: LED%u\n", selected + 1);
}

// giu nut: LED dang chon nhap nhay
void btnHold()
{
    leds[selected]->blink(BLINK_MS);
    Serial.printf("LED%u: blink %d ms\n", selected + 1, BLINK_MS);
}
