#include <cstdint>
#include "pinDef.h"
#ifndef pin_size_t
typedef uint8_t pin_size_t;
#endif

class Battery {
    public:
        void init();
        float readVoltage();
        bool isCharging();
        bool isFull();
        bool isVBusConnected();
};