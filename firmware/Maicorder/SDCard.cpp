#include "SDCard.h"
#include "config.h"
#include "pinDef.h"


#include <SPI.h>
#include <SDFS.h>
#include <cmath>


bool SDCard::isInserted() {
    return !digitalRead(SD_CD);
}

void SDCard::init() {
    pinMode(SD_CD, INPUT);
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

bool SDCard::createNewFile(time_t timeStamp) {
    if (!this->isInserted()) {
        return false;
    }
    struct tm startTime;
    gmtime_r(&timeStamp, &startTime);
    char fileName[20]; // file name (YYYYMMDD_HHMMSS.wav)
    sprintf(fileName, "%.4d%.2d%.2d_%.2d%.2d%.2d.wav", 
                startTime.tm_year + 1900, startTime.tm_mon + 1, startTime.tm_mday,
                startTime.tm_hour + TIMEZONE_OFFSET, startTime.tm_min, startTime.tm_sec);
    this->waveFile = SDFS.open(fileName, "w+");
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

bool SDCard::updateFile() {
    if (!this->waveFile) {
        return false;
    }
    if(!this->waveFile.write(this->pcmBuffer, AUDIO_TRANSMIT_THRESHOLD)) { // result of write data
        return false;
    }
    this->header.chunkSize += AUDIO_TRANSMIT_THRESHOLD;
    this->header.subChunk2Size += AUDIO_TRANSMIT_THRESHOLD;
    if (millis() - this->lastFlushTime > SD_FLUSH_SEGMENT) {
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