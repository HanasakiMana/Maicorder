#include "pinDef.h"
#include "config.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace I2C {
    void init();
    bool write(uint8_t addr, uint8_t reg, uint8_t data);
    uint8_t read(uint8_t addr, uint8_t reg);
};