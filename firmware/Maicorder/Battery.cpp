#include "Battery.h"
#include <Arduino.h>

void Battery::init() {
    pinMode(VBUS_DETECT, INPUT_PULLDOWN);
    pinMode(BATTERY_ADC_EN, OUTPUT);
    pinMode(BATTERY_CHRG, INPUT_PULLUP);
    pinMode(BATTERY_FULL, INPUT_PULLUP);
    digitalWrite(BATTERY_ADC_EN, LOW);
    analogReadResolution(12); // 12-bit resolution
}

float Battery::readVoltage() {
    digitalWrite(BATTERY_ADC_EN, HIGH);
    delay(100); // wait for ADC to be stable
    // voltage divider = 2/3, Vref = 3.3V
    float voltage = ((float)analogRead(BATTERY_ADC))/4095*3.3f*3/2;
    digitalWrite(BATTERY_ADC_EN, LOW);
    return voltage;
}

bool Battery::isCharging() {
    if (!digitalRead(BATTERY_CHRG)) return true;
    else return false;
}

bool Battery::isFull() {
    if (!digitalRead(BATTERY_FULL)) return true;
    else return false;
}

bool Battery::isVBusConnected() {
    if (digitalRead(VBUS_DETECT)) return true;
    else return false;
}