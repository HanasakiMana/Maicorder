#include <cstdint>
#include "config.h"
#include "pinDef.h"

class RGB {
    public:
        uint8_t R = LED_R;
        uint8_t G = LED_G;
        uint8_t B = LED_B;
        uint32_t freq = PWM_FREQ;
        int resolution = PWM_RESOLUTION;
        void init();
        void setColor(uint32_t color);
};