#include "sd_card.h"

bool sdCard::init() {
    this->card.setDx(SDIO_D0, SDIO_D1, SDIO_D2, SDIO_D3);
    this->card.setCMD(SDIO_CMD);
    this->card.setCK(SDIO_CLK);
    if(!this->card.begin(SDIO_CD, LOW)) return false;
    return true;
}

bool sdCard::createNewFile() {
    for (int index=0; index<MAX_FILE_SERIAL; index++) { // choose a new serial number as file name
        char fileName[(unsigned int)std::log2(MAX_FILE_SERIAL) + 1];
        sprintf(fileName, "%d.wav", index);
        if (!this->card.exists(fileName)) {
            this->waveFile = this->card.open(fileName, FILE_WRITE);
            // result of create file
            if(!this->waveFile) { 
                return false;
            }
            // write file header
            this->waveFile.write((uint8_t*)&this->header, sizeof(wavHeader));
            this->flush();
            this->lastFlushTime = millis();
            return true;
        }
    }
    return false;
}

bool sdCard::updateFile(uint8_t* buffer, size_t size) {
    if (!this->waveFile) return false;
    this->waveFile.write(buffer, size);
    // if (!) return false;
    this->header.chunkSize += size;
    this->header.subChunk2Size += size;
    if (millis() - this->lastFlushTime >= this->flushSegment) {
        this->flush();
    }
    return true;
}

void sdCard::flush() {
    this->waveFile.flush();
    this->lastFlushTime = millis();
}

void sdCard::closeFile() {
    if (this->waveFile) {
        this->waveFile.seek(0); // rewrite file header
        this->waveFile.write((uint8_t *)&this->header, sizeof(this->header));
        this->flush();
        this->waveFile.close();
    }
}