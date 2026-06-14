#include "pcm1863.h"
#include "config.h"
#include "pinDef.h"
#include "beep.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <I2S.h>
#include <Arduino.h>

void PCM1863::i2cInit() {
    i2c_init(i2c0, this->i2cClk);
    gpio_set_function(this->SDA, GPIO_FUNC_I2C);
    gpio_set_function(this->SCL, GPIO_FUNC_I2C);
    gpio_pull_up(this->SDA);
    gpio_pull_up(this->SCL);
    delay(100);
    // set to page 0
    this->i2cWrite(0x00, 0x00);
    // P0_R32: set clock
    // SCK as clock input (D7~D6=00), Slave mode (D4), Auto clock detector disabled (D0=0)
    // set all DSP and ADC clock source to PLL (D3~D1=111)
    // keep other bits as default
    this->i2cWrite(0x20, 0b00001110);
    // P0_R33: DSP1 clock divider
    this->i2cWrite(0x21, 7); // 1/8
    // P0_R34: DSP2 clock divider
    this->i2cWrite(0x22, 7); // 1/8
    // P0_R35: ADC clock divider
    this->i2cWrite(0x23, 15); // 1/16
    // P0_R40: PLL init     ?????
    // PLL is locked(D4=0), PLL reference is BCK(D1=1), PLL enable(D0=1) 
    this->i2cWrite(0x28, 0b00010011);
    // P0_R41: PLL P divider
    this->i2cWrite(0x29, 0); // P = 1
    // P0_R42: PLL R multiplier
    this->i2cWrite(0x2A, 3); // R = 2
    // P0_R43: PLL J.D multiplier
    this->i2cWrite(0x2B, 16); // J = 16
    // D = 0 by default, nothing to change

    // P0_R5: set PGA and Automatic Clipping
    // enable PGA smooth control (D7=1), let other channels follow CH1[L] PGA gain (D6=1)
    // Enable clipping detection after DPGA (D5=1)
    // Limit of Auto Clipping Suppression (D4-D3)
    // trigger auto clipping suppression after CLIP_NUMs of clipping events (D2-D1)
    // enable auto clipping suppression (D0=1) 
    this->i2cWrite(0x05, 0b111 << 5 | this->autoClippingLimit << 3 | this->clippingEventNum << 1 | 1);
    // P0_R11: set PCM format
    // Stereo PCM Word Length 16-bit (D3-2=0b11), I2S format (D1-D0=0b00)
    // keep other bits as default
    this->i2cWrite(0x0B, 0b01001100);
    
    // set PGA gain
    this->setPgaGain(PGA_GAIN_DEFAULT);
}

int PCM1863::i2cWrite(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    int result = i2c_write_timeout_us(i2c0, this->i2cAddr, buf, 2, false, 500000);
    if (result<0) beep(100);
    return result;
}

void PCM1863::setPgaGain(uint8_t gain) {
    // set to page 0
    int result = this->i2cWrite(0x00, 0x00);
    
    // set PGA gain, all channels follow Ch1[L]
    this->i2cWrite(0x01, gain);
}

void PCM1863::init() {
    this->i2sInit();
    // init i2c device
    delay(100); // wait for device to be ready
    this->i2cInit();

}

void PCM1863::i2sInit() {
    this->i2s.setBCLK(this->BCLK);
    this->i2s.setDIN(this->DIN);
    // this->i2s.setMCLK(this->MCLK);
    this->i2s.setBitsPerSample(16);
    this->i2s.setFrequency(48000);
    this->i2s.setSysClk(48000); // optimize system clock to meet sample rate
    // this->i2s.setMCLKmult(256); // MCLK = 48kHz * 256 = 12.288MHz
    this->i2s.setBuffers(this->bufferNum, this->bufferSize);
    this->i2s.begin();
}

void PCM1863::updateBuffer(uint8_t* buffer, size_t size) {
    while (true){
        if (this->i2s.available() >= size) {
            this->i2s.read(buffer, size);
            break;
        }
    }
}

