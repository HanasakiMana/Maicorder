#include "pinDef.h"
#include "config.h"
#include "wave.h"
#include <SDFS.h>

class SDCard {
    public:
        // buffer settings
        unsigned long audioTransmitThreshold = AUDIO_TRANSMIT_THRESHOLD;
        unsigned int flushSegment = SD_FLUSH_SEGMENT;
        // buffer
        __attribute__((aligned(4))) uint8_t pcmBuffer[AUDIO_TRANSMIT_THRESHOLD];
        
        void init();
        bool begin();
        FSInfo getInfo();
        bool isInserted();
        bool createNewFile();
        bool updateFile();
        void close();
    
    private:
        wavHeader header;
        File waveFile;
        FSInfo sdInfo;
        unsigned long lastFlushTime = 0;
        void spiInit();
        void flush();
};