#include "pinDef.h"
#include "config.h"
#include "wave.h"

#include <SDFS.h>

class SDCard {
    public:
        // buffer
        __attribute__((aligned(4))) uint8_t pcmBuffer[AUDIO_TRANSMIT_THRESHOLD];
        
        void init();
        bool begin();
        FSInfo getInfo();
        bool isInserted();
        bool createNewFile(time_t startTime);
        bool updateFile();
        void close();
    
    private:
        wavHeader header;
        File waveFile;
        FSInfo sdInfo;
        unsigned long lastFlushTime = 0;
        void flush();
};