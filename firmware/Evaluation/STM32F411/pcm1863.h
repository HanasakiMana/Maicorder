#include "pin_def.h"
#include <cstdint>
#include <Wire.h>
#include <arm_math.h>
#include <Arduino.h>


#ifndef pin_size_t
typedef uint8_t pin_size_t;
#endif

#define AUDIO_I2C_ADDR 0x4A
#define PGA_GAIN_DEFAULT 36.0f // dB
#define AUTO_CLIPPING_LIMIT_DEFAULT 0b00 // -3dB
#define CLIPPING_EVENT_NUM_DEFAULT 0b11 // 10 clipping events

#define I2S_BUFFER_SIZE 32768 // samples, or 64KB

#define MIN_GAIN -12.0f // dB
#define MAX_GAIN 40.0f // dB
#define GAIN_STEP 0.5f // dB
#define TARGET_LOUDNESS_DBFS -6.0f // dB
#define GAIN_CHANGE_GATE 2.0f // dB
#define LOW_VOLUME_THRESHOLD -60.0f // dB

// normalized RMS[0.0f, 1.0f) = 10^(dbFS/20)
// dbFS = 20 * log10f(RMS)
// q15_t/32768 = normalized RMS [-1.0f, 1.0f)


class PCM1863 {
    public:
        // i2c configs
        uint8_t i2cAddr = AUDIO_I2C_ADDR;
        pin_size_t SDA = I2C1_SDA;
        pin_size_t SCL = I2C1_SCL;
        // i2s configs
        unsigned int bufferSize = I2S_BUFFER_SIZE;
        // PGA
        int8_t pgaGain = PGA_GAIN_DEFAULT;
        uint8_t autoClippingLimit = AUTO_CLIPPING_LIMIT_DEFAULT;
        uint8_t clippingEventNum = CLIPPING_EVENT_NUM_DEFAULT;
        float minGain = MIN_GAIN;
        float maxGain = MAX_GAIN;
        float gainStep = GAIN_STEP;
        float targetLoudness = TARGET_LOUDNESS_DBFS;
        float gainChangeGate = GAIN_CHANGE_GATE;
        float lowVolumeThreshold = LOW_VOLUME_THRESHOLD;

    
        uint16_t audioBuffer[I2S_BUFFER_SIZE] __attribute__((aligned(4))) = {0};
        volatile bool firstHalfFull = false;
        volatile bool secondHalfFull = false;

        


        void init();
        void setPgaGain(int8_t gain);
        void initDMA(I2S_HandleTypeDef hi2s);
        void updateGain();
        
    
    private:
        TwoWire i2c = TwoWire(I2C1_SDA, I2C1_SCL);
        void i2cWrite(uint8_t reg, uint8_t data);
        uint8_t i2cRead(uint8_t reg);

        // circular array to record past loudness value in dBFS
        float pastLoudness[20] = {0.0f};
        int pastLoudnessPtr = 0;
        void pushNewLoudness(float loudness);
        
};