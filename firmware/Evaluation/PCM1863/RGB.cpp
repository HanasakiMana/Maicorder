#include "RGB.h"
#include <cstdint>
#include <Arduino.h>

void RGB::init() {
    analogWriteFreq(this->freq);
    analogWriteResolution(this->resolution);
}

void RGB::setColor(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    analogWrite(this->R, r);
    analogWrite(this->G, g);
    analogWrite(this->B, b);
}
