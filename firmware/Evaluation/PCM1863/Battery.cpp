#include "Battery.h"
#include <Arduino.h>

void Battery::init() {
    pinMode(this->adcEn, OUTPUT);
    digitalWrite(this->adcEn, LOW);
    analogReadResolution(12); // 12-bit resolution
}

float Battery::readVoltage() {
    digitalWrite(this->adcEn, HIGH);
    delay(10); // wait for ADC to stabilize
    // voltage divider = 2/3, Vref = 3.3V
    float voltage = ((float)analogRead(this->adc))/4095*3.3*3/2;
    digitalWrite(this->adcEn, LOW);
    return voltage;
}