#include "pcm1863.h"



void PCM1863::init() {
    this->i2c.begin();
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
    // P0_R40: PLL init
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
    this->setPgaGain((int8_t)this->pgaGain*2);

}

void PCM1863::i2cWrite(uint8_t reg, uint8_t data) {
    this->i2c.beginTransmission(this->i2cAddr);
    this->i2c.write(reg);
    this->i2c.write(data);
    this->i2c.endTransmission();
}

void PCM1863::setPgaGain(int8_t gain) {
    // set to page 0
    this->i2cWrite(0x00, 0x00);
    // eliminate illegal gain
    if (gain > (int8_t)(this->maxGain*2)) gain = (int8_t)(this->maxGain*2);
    if (gain < (int8_t)(this->minGain*2)) gain = (int8_t)(this->minGain*2);
    // set PGA gain, all channels will follow Ch1[L]
    this->i2cWrite(0x01, gain);
}

void PCM1863::updateGain() {
    uint16_t histogram[256] = {0};
    q15_t buffer[1024] = {0};
    uint16_t index = 0;
    if (this->firstHalfFull && !this->secondHalfFull) index = 0;
    if (this->secondHalfFull && !this->firstHalfFull) index = this->bufferSize/2;
    for (int i=0; i<this->bufferSize/2/256; i++) {
        arm_abs_q15((q15_t *)this->audioBuffer[index + i*1024], buffer, 1024);
        for (int j=0; j<1024; j++) histogram[buffer[j]>>7]++;
    }
    
    // prevent windup of this->pgaGain
    if (this->pgaGain > this->maxGain) this->pgaGain = this->maxGain;
    if (this->pgaGain < this->minGain) this->pgaGain = this->minGain;
    this->setPgaGain((int8_t)(this->pgaGain*2)); 
}