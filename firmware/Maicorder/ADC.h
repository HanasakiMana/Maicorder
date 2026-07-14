#include "config.h"
#include "pinDef.h"
#include "I2C.h"

#include <cstdint>
#include <I2S.h>
#include "arm_math.h"

class ADC {
    public:
        int8_t currentGain = 0;

        void init();
        void setGain(int8_t gain);
        void changeGain(float deltaGain);
        float updateBuffer(uint8_t* buffer, size_t size);
        void clearBuffer();

    private:
        I2S i2s = I2S(INPUT);
        uint8_t gainSelect = 0;
        
        void i2sInit();
        void i2cInit();
        float rms2dbfs(q15_t rms);
};