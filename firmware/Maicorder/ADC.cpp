#include "ADC.h"

void ADC::i2sInit() {
    this->i2s.setBCLK(I2S_BCLK);
    this->i2s.setDIN(I2S_DIN);
    this->i2s.setBitsPerSample(16);
    this->i2s.setFrequency(48000);
    this->i2s.setSysClk(48000); // optimize system clock to meet sample rate
    this->i2s.setBuffers(I2S_BUFFER_NUM, I2S_BUFFER_SIZE);
    this->i2s.begin();
}

void ADC::i2cInit() {
    delay(100); // wait for PCM186x to power up
    I2C::write(ADC_ADDR, 0x00, 0x00); // page 0
    // ---------- Clock and PLL settings ----------
    // P0_R32: set clock
    // SCK as clock input (D7~D6=00), Slave mode (D4), auto clock detector disabled (D0=0)
    // set all DSP and ADC clock source to PLL (D3~D1=111)
    I2C::write(ADC_ADDR, 0x20, 0b00001110);
    // P0_R33: DSP1 clock divider
    I2C::write(ADC_ADDR, 0x21, 7); // 1/8
    // P0_R34: DSP2 clock divider
    I2C::write(ADC_ADDR, 0x22, 7); // 1/8
    // P0_R35: ADC clock divider
    I2C::write(ADC_ADDR, 0x23, 15); // 1/16
    // P0_R40: PLL init     ?????
    // PLL is locked(D4=0), PLL reference is BCK(D1=1), PLL enable(D0=1) 
    I2C::write(ADC_ADDR, 0x28, 0b00010011);
    // P0_R41: PLL P divider
    I2C::write(ADC_ADDR, 0x29, 0); // P = 1
    // P0_R42: PLL R multiplier
    I2C::write(ADC_ADDR, 0x2A, 3); // R = 2
    // P0_R43: PLL J.D multiplier
    I2C::write(ADC_ADDR, 0x2B, 16); // J = 16
    // D = 0 by default, nothing to change
    // ---------- ADC settings ----------
    // P0_R5: set PGA and Automatic Clipping
    // enable PGA smooth control (D7=1), let other channels follow CH1[L] PGA gain (D6=1)
    // Enable clipping detection after DPGA (D5=1)
    // Limit of Auto Clipping Suppression (D4-D3)
    // trigger auto clipping suppression after CLIP_NUMs of clipping events (D2-D1)
    // enable auto clipping suppression (D0=1) 
    I2C::write(ADC_ADDR, 0x05, 0b11100111);
    // P0_R11: set PCM format
    // Stereo PCM Word Length 16-bit (D3-2=0b11), I2S format (D1-D0=0b00)
    // keep other bits as default
    I2C::write(ADC_ADDR, 0x0B, 0b01001100);
    // reset clipping flag
    I2C::write(ADC_ADDR, 0x60, 0x10);

    // set PGA gain
    if (this->gainSelect == 0) this->setGain(GAIN_LEVEL_1);
    else if (this->gainSelect == 1) this->setGain(GAIN_LEVEL_2);
    else if (this->gainSelect == 2) this->setGain(GAIN_LEVEL_3);
    else if (this->gainSelect == 3) this->setGain(GAIN_LEVEL_4);
}

// Notice: gain = realGain*2, since the gain step is 0.5dB
void ADC::setGain(int8_t gain) {
    I2C::write(ADC_ADDR, 0x00, 0x00); // go to page 0
    if (gain > 80) gain = 80; // max gain = 40dB
    if (gain < -24) gain = -24; // min gain = -12dB
    I2C::write(ADC_ADDR, 0x01, gain);
    this->currentGain = gain;
}

void ADC::init() {
    pinMode(SEL1, INPUT_PULLUP);
    pinMode(SEL2, INPUT_PULLUP);
    pinMode(SEL3, INPUT_PULLUP);
    pinMode(SEL4, INPUT_PULLUP);
    pinMode(ALDO_EN, OUTPUT);
    digitalWrite(ALDO_EN, HIGH); // enable analog LDO
    if (!digitalRead(SEL1)) this->gainSelect = 0;
    else if (!digitalRead(SEL2)) this->gainSelect = 1;
    else if (!digitalRead(SEL3)) this->gainSelect = 2;
    else if (!digitalRead(SEL4)) this->gainSelect = 3;
    this->i2sInit();
    this->i2cInit();
}

// will return peak value in dBFS
float ADC::updateBuffer(uint8_t* buffer, size_t size) {
    while (true) {
        if (this->i2s.available() >= size) {
            this->i2s.read(buffer, size);
            // get rms
            q15_t peak = 0;
            arm_absmax_q15((q15_t *) buffer, size/2, &peak, nullptr);
            return 20.0f * log10f((float)peak/32768);
            break;
        }
    }
}

float ADC::rms2dbfs(q15_t rms) {
    if (rms == 0) return -96.0; // minimum value of 16bit PCM
    return 20.0f * log10f((float)rms/RMS_0DBFS_Q15);
}


void ADC::clearBuffer() {
    this->i2s.end();
    while (this->i2s.available()) this->i2s.read();
    this->i2s.begin();
}

void ADC::changeGain(float deltaGain) {
    int8_t targetGain = (int8_t)round(((float)this->currentGain + deltaGain*2));
    I2C::write(ADC_ADDR, 0x00, 0x00);
    I2C::write(ADC_ADDR, 0x01, targetGain);
    this->currentGain = targetGain;
}