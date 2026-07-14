#include "SDCard.h"
#include "config.h"
#include "pinDef.h"


#include <SPI.h>
#include <SDFS.h>
#include <cmath>

void SDCard::spiInit() {
    /*SPI1.setRX(this->MISO);
    SPI1.setTX(this->MOSI);
    SPI1.setSCK(this->SCK);
    SPI1.setCS(this->CS);*/
}

bool SDCard::isInserted() {
    return !digitalRead(SD_CD);
}

void SDCard::init() {
    pinMode(SD_CD, INPUT);
    // this->spiInit();
}

bool SDCard::begin() {
    if (this->isInserted()) {
        SDFSConfig conf = SDFSConfig(SD_CLK, SD_CMD, SD_D0);
        conf.setSPISpeed(SD_SCK_MHZ(25));
        SDFS.setConfig(conf);
        // SDFS.setConfig(SDFSConfig(this->CS, SD_SCK_MHZ(25), SPI1));
        return SDFS.begin();
    }
    return false;
}

FSInfo SDCard::getInfo() {
    SDFS.info(this->sdInfo);
    return this->sdInfo;
}

bool SDCard::createNewFile() {
    if (!this->isInserted()) {
        return false;
    }
    for (int index=0; index<MAX_FILE_SERIAL; index++) { // choose a new serial number as file name
        char fileName[(unsigned int)std::log2(MAX_FILE_SERIAL) + 1];
        sprintf(fileName, "%d.wav", index);
        if (!SDFS.exists(fileName)) {
            this->waveFile = SDFS.open(fileName, "w+");
            // result of create file
            if(!this->waveFile) { 
                return false;
            }
            // write file header
            this->waveFile.write((uint8_t*)&this->header, sizeof(wavHeader));
            this->lastFlushTime = millis();
            return true;
        }
    }
    return false;
}

bool SDCard::updateFile() {
    if (!this->waveFile) {
        return false;
    }
    if(!this->waveFile.write(this->pcmBuffer, this->audioTransmitThreshold)) { // result of write data
        return false;
    }
    this->header.chunkSize += this->audioTransmitThreshold;
    this->header.subChunk2Size += this->audioTransmitThreshold;
    if (millis() - this->lastFlushTime > this->flushSegment) {
        this->flush();
        this->lastFlushTime = millis();
    }
    return true;
}

void SDCard::flush() {
    this->waveFile.flush();
}

void SDCard::close() {
    if (this->waveFile) {
        // overwrite header with correct size
        this->waveFile.seek(0, SeekSet);
        this->waveFile.write((uint8_t*)&this->header, sizeof(wavHeader));
        this->flush();
        this->waveFile.close();
    }
}