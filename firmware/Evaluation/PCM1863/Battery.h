#include <cstdint>
#include "pinDef.h"
#ifndef pin_size_t
typedef uint8_t pin_size_t;
#endif

class Battery {
    public:
        pin_size_t adc = BATTERY_ADC;
        pin_size_t adcEn = BATTERY_ADC_EN;
        void init();
        float readVoltage();
};