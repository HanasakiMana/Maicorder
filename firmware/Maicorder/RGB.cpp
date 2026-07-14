#include "RGB.h"
#include <cstdint>
#include <Arduino.h>

void RGB::init() {
    analogWriteFreq(PWM_FREQ);
    analogWriteResolution(PWM_RESOLUTION);
}

void RGB::setColor(uint32_t color) {
    uint8_t r = (~color >> 16) & 0xFF;
    uint8_t g = (~color >> 8) & 0xFF;
    uint8_t b = ~color & 0xFF;
    analogWrite(LED_R, r);
    analogWrite(LED_G, g);
    analogWrite(LED_B, b);
}

void RGB::blink(uint32_t color, uint16_t interval, uint8_t repeatCount) {
    for (uint8_t i = 0; i < repeatCount; i++) {
        this->setColor(color);
        delay(interval);
        this->setColor(0);
        delay(interval);
    }
}