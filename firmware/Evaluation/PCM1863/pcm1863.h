#include "config.h"
#include "pinDef.h"
#include <cstdint>
#include <I2S.h>


#ifndef pin_size_t
typedef uint8_t pin_size_t;
#endif

class PCM1863 {
    public:
        // i2c configs
        uint8_t i2cAddr = AUDIO_I2C_ADDR;
        pin_size_t SDA = PCM1863_SDA;
        pin_size_t SCL = PCM1863_SCL;
        unsigned int i2cClk = AUDIO_I2C_CLK;
        // i2s configs
        pin_size_t DIN = I2S_DIN;
        pin_size_t DOUT = I2S_DOUT;
        pin_size_t BCLK = I2S_BCLK;
        pin_size_t MCLK = I2S_MCLK;
        unsigned int bufferNum = I2S_BUFFER_NUM;
        unsigned int bufferSize = I2S_BUFFER_SIZE;
        // PGA
        uint8_t pgaGain = PGA_GAIN_DEFAULT;
        uint8_t autoClippingLimit = AUTO_CLIPPING_LIMIT_DEFAULT;
        uint8_t clippingEventNum = CLIPPING_EVENT_NUM_DEFAULT;
    
        void init();
        void setPgaGain(uint8_t gain);
        void updateBuffer(uint8_t* buffer, size_t size);
    
    private:
        // i2s object
        I2S i2s = I2S(INPUT); // INPUT

        void i2cInit();
        int i2cWrite(uint8_t reg, uint8_t data);
        void i2sInit();
        
};