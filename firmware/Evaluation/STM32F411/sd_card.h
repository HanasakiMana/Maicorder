#include <cstdint>
#include <STM32SD.h>
#include "pin_def.h"

#define SD_HW_FLOW_CTRL_ENABLE



// serial number in file name *.wav
#define MAX_FILE_SERIAL 65534

#define forceFlushSegment 2000 // ms

typedef struct {
  // RIFF chunk
  uint8_t chunkID[4] = {'R', 'I', 'F', 'F'};
  uint32_t chunkSize = 36; // full length - 8
  uint8_t format[4] = {'W', 'A', 'V', 'E'};
  // Format chunk
  uint8_t subChunk1ID[4] = {'f', 'm', 't', ' '};
  uint32_t subChunk1Size = 16;
  uint16_t audioFormat = 1; // PCM
  uint16_t numChannels = 2; // stereo
  uint32_t sampleRate = 48000;
  uint32_t byteRate = 192000; // 48000Hz * 2 channel * 16bits (2Byte)
  uint16_t blockAlign = 4; // 2 channel * 16bits (2Byte)
  uint16_t bitsPerSamlple = 16;
  // Data chunk
  uint8_t subChunk2ID[4] = {'d', 'a', 't', 'a'};
  uint32_t subChunk2Size = 0; // data length
} wavHeader;


class sdCard {
    public:
        bool init();
        bool createNewFile();
        bool updateFile(uint8_t* buffer, size_t size);
        void closeFile();

    private:
        SDClass card;
        File waveFile;
        unsigned long lastFlushTime = 0;
        wavHeader header;

        unsigned int flushSegment = forceFlushSegment;
        void flush();
};

