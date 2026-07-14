#include "I2C.h"

void I2C::init() {
    i2c_init(i2c0, I2C_CLK);
    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);
}


bool I2C::write(uint8_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    int result = i2c_write_timeout_us(i2c0, addr, buf, 2, false, 500000);
    return result >= 0;
}

uint8_t I2C::read(uint8_t addr, uint8_t reg) {
    i2c_write_timeout_us(i2c0, addr, &reg, 1, false, 500000);
    uint8_t data;
    int result = i2c_read_timeout_us(i2c0, addr, &data, 1, false, 500000);
    if (result < 0) return 0xFF;
    return data;
}