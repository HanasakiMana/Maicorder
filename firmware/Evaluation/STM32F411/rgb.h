#include <cstdint>
#include "pin_def.h"

#define PWM_FREQ 1000

// TIM3:
//    CH1: Green
//    CH3: Blue
//    CH4: Red

class RGB {
    public:
        uint32_t freq = PWM_FREQ;
        void init();
        void setColor(uint32_t color);

    private:
        uint8_t R = LED_R;
        uint8_t G = LED_G;
        uint8_t B = LED_B;
        uint32_t chR = 4;
        uint32_t chG = 1;
        uint32_t chB = 3;
        HardwareTimer *timer;
};