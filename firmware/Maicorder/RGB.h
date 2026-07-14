#include <cstdint>
#include "config.h"
#include "pinDef.h"

class RGB {  
    public:
        void init();
        void setColor(uint32_t color);
        void blink(uint32_t color, uint16_t interval, uint8_t repeatCount);
};