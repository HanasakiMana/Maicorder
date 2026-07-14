#include "config.h"
#include "pinDef.h"
#include <cstdint>
#include <I2S.h>

#include "arm_math.h"

#ifndef pin_size_t
typedef uint8_t pin_size_t;
#endif

class PCM1863 {
    public:
        // i2c configs
        uint8_t i2cAddr = AUDIO_I2C_ADDR;
        // i2s configs
        pin_size_t DIN = I2S_DIN;
        pin_size_t BCLK = I2S_BCLK;
        unsigned int bufferNum = I2S_BUFFER_NUM;
        unsigned int bufferSize = I2S_BUFFER_SIZE;
        // PGA
        uint8_t pgaGain = PGA_GAIN_DEFAULT;
        uint8_t autoClippingLimit = AUTO_CLIPPING_LIMIT_DEFAULT;
        uint8_t clippingEventNum = CLIPPING_EVENT_NUM_DEFAULT;
    
        void init();
        void setPgaGain(uint8_t gain);
        void updateBuffer(uint8_t* buffer, size_t size);
        uint8_t i2cRead(uint8_t reg);
        int i2cWrite(uint8_t reg, uint8_t data);
        bool isClipping();
        I2S i2s = I2S(INPUT); // INPUT
    
    private:
        // i2s object
        void i2sInit();
        void i2cInit();
        
};

class AGC {
    public:
        float rms2dbfs(q15_t rms);
        float getDeltaGain(q15_t currentRMS, float target);

};