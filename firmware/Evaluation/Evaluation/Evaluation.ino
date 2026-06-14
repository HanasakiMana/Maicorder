#include "pinDef.h"
#include "config.h"
#include "wave.h"

#include <I2S.h>
#include <SDFS.h>

#include "hardware/i2c.h"
#include "hardware/gpio.h"

bool core1_separate_stack = true; // give every core 8k of stack size

I2S audio(INPUT_PULLUP);

void audioInit() {
  // ---------- Page 0 ----------
  sendData(AUDIO_I2C_ADDR, 0x00, 0x00); // select Page 0
  sendData(AUDIO_I2C_ADDR, 0x01, 0x01); // software reset
  delay(100);

  // ----- PLL -----
  
  // while fs=48kHz, MCLK=3.072MHz, configurations should be set as:
  // PLLP=1, PLLR=4, PLLJ=7, PLLD=0 => PLL_CLK = 3.072MHz * (4 * 7.0)/1 = 86.016MHz
  // MADC=2, NADC=7, AOSR=128, MDAC=7, NDAC=2, DOSR=128
  // see page 78 on SLAA557
  
  // P0_R4, Clock Multiplexers
  // Low PLL Clock Range (80MHz~132MHz@AVDD>1.8V), MCLK as PLL input, PLL as CODEC Input
  // maintain default values
  sendData(AUDIO_I2C_ADDR, 0x04, 0b00000011);
  

  // P0_R6, PLLJ (D5~D0)
  // PLLJ=7
  sendData(AUDIO_I2C_ADDR, 0x06, 0b00000111);

  // P0_R7 PLLD (MSB)
  // P0_R8 PLLD (LSB)
  // PLLD=0, maintain default values

  // P0_R5, PLLP & PLLR
  // power up the PLL (D7), PLLP=1 (D6~D4), PLLR=4 (D3~D0)
  sendData(AUDIO_I2C_ADDR, 0x05, 0b10010100);

  delay(10);

  // P0_R11, NDAC
  // power up the NDAC (D7), NDAC=2 (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x0b, 0b10000010);

  // P0_R12, MDAC
  // power up the MDAC (D7), MDAC=7 (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x0c, 0b10000111);

  // P0_R13, DAC Oversampling MSB (D1~D0)
  // P0_R14, DOSR LSB (D7~D0)
  // R14 should be written immediately after R13
  sendData(AUDIO_I2C_ADDR, 0x0d, 0b00000000);
  sendData(AUDIO_I2C_ADDR, 0x0e, 0b10000000);

  // P0_R18, NADC
  // power up the NADC (D7), NADC=7 (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x12, 0b10000111);

  // P0_R19, MADC
  // power up the MADC (D7), MADC=2 (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x13, 0b10000010);

  // P0_R20, ADC Oversampling (AOSR)
  // AOSR=128, maintain default value

  // ----- I2S -----
  
  // P0_R27, Audio Interface
  // Mode: I2S (D7-D6), Word length: 16bits (D5~D4)
  // BCLK slave (D3), WCLK slave (D2)
  // keep high impendance on DOUT after data has been transfered
  sendData(AUDIO_I2C_ADDR, 0x1b, 0b00110001);

  // ----- Digital Control -----

  // P0_R29, Audio Interface
  // no Audio Data Loopback (D5=0), open ADC->DAC Loopback (D4=1)
  // others keep default (0)
  sendData(AUDIO_I2C_ADDR, 0x1d, 0b00010000);

  // ----- ADC -----
  // P0_R61, ADC Signal Processing Block
  // PB=PRB_R1 (D4~D0)
  // Stereo, Decimation Filter A (for fs=48kHz), 0 BiQuads
  // No FIR
  sendData(AUDIO_I2C_ADDR, 0x3d, 0b00000001);

  // P0_R81, ADC Channel Setup
  // power up left (D7) & right (D6) channel
  // use GPIO as Mic input (D5~D4)
  // left (D3) and right (D2) ADC are not configured for Mic
  // volume changes by 1 gain step per WCLK (D1~D0)
  sendData(AUDIO_I2C_ADDR, 0x51, 0b11000000);

  // P0_R82, ADC Fine Gain Adjust
  // ADC un-muted (left D7, right D3)
  // gain=0dB (left D6~D4, right D2~D0)
  sendData(AUDIO_I2C_ADDR, 0x52, 0b00000000);

  // P0_R83, Left ADC Volume Control
  // 0.0dB, keep default

  // P0_R84, Right ADC Volume Control
  // 0.0dB, keep default

  // P0_R86, Left AGC (Automatic Gain Control)
  // P0_R94, Right AGC
  // Enable AGC (D7), Target level (D6~D4)
  // Gain Hysteresis Control (D1~D0)
  sendData(AUDIO_I2C_ADDR, 0x56, 0b1 << 7 | AGC_TARGET << 4 | AGC_GAIN_HYSTERESIS);
  sendData(AUDIO_I2C_ADDR, 0x5e, 0b1 << 7 | AGC_TARGET << 4 | AGC_GAIN_HYSTERESIS);

  // P0_R87, Left AGC
  // P0_R95, Right AFC
  // Hysteresis (D7~D6), Noise Threshold (D5~D1)
  // keep default

  // P0_R88, Left AGC
  // P0_R96, Right AGC
  // AGC Maximum Gain (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x58, AGC_MAX_GAIN);
  sendData(AUDIO_I2C_ADDR, 0x60, AGC_MAX_GAIN);

  // P0_R89, Left AGC
  // P0_R97, Right AGC
  // AGC Attack (D7-D3), Scale (D2~D0)
  sendData(AUDIO_I2C_ADDR, 0x59, AGC_ATK << 3 | AGC_ATK_SCALE);
  sendData(AUDIO_I2C_ADDR, 0x61, AGC_ATK << 3 | AGC_ATK_SCALE);

  // P0_R90, Left AGC
  // P0_R98, Right AGC
  // AGC Decay Time (D7~D3), Scale (D2~D0)
  sendData(AUDIO_I2C_ADDR, 0x5A, AGC_DECAY << 3 | AGC_DECAY_SCALE);
  sendData(AUDIO_I2C_ADDR, 0x62, AGC_DECAY << 3 | AGC_DECAY_SCALE);

  // P0_R91, Left AGC
  // P0_R99, Right AGC
  // AGC Noise Debounce (D4~D0)
  // keep default

  // P0_R92, Left AGC
  // P0_R100, Right AGC
  // AGC Singal Debounce (D3~D0)
  // keep default

  // ----- DAC -----
  // P0_R63, DAC Setup
  // L&R DAC power up (D7=D6=1), keep others default (00010100)
  sendData(AUDIO_I2C_ADDR, 0x3f, 0b11010100);

  // P0_R64, DAC Setup
  // L&R DAC un-muted (D3=D2=0), keep others default (all 0)
  // sendData(AUDIO_I2C_ADDR, 0x40, 0b00000000);

  // ---------- Page 1 ----------
  sendData(AUDIO_I2C_ADDR, 0x00, 0x01); // switch to page 1
  
  // ----- Analog Config -----
  
  // P1_R1, Power Config
  // disable connection of AVDD and DVDD (D3)
  sendData(AUDIO_I2C_ADDR, 0x01, 0b00001000);

  // P1_R2, LDO
  // DVDD LDO volt (D7~D6), AVDD LDO volt (D5~D4)
  // open Analog Block Power (D3=0)
  // DVDD OC detect (D2), AVDD OC detect (D1)
  // open AVDD LDO (D0=1)
  sendData(AUDIO_I2C_ADDR, 0x02, 0b00000001);

  // P1_R3, left playback
  // P1_R4, right playback
  // DAC->HP with Class-D driver (D7~D6), keep default
  // Power Tune Mode (PTM) (D4~D2), keep default

  // P1_R9, Output Driver
  // open HPL power (D5=1)
  // open HPR power (D4=1)
  // open LOL power (D3=1)
  // open LOR power (D2=1)
  // open Left Mixer power (D1=1)
  // open Right Mixer power (D0=1)
  sendData(AUDIO_I2C_ADDR, 0x09, 0b00111111);

  // P1_R10, Common Mode Control
  // CM=0.9V (D6=0), HP_CM=1.65V (D5~D4=11)
  // LO_CM=1.65V (D3=1)
  // HP is powered by LDOIN (D1=1)
  // LDOIN input range is 1.8V~3.6V (D0=1)
  sendData(AUDIO_I2C_ADDR, 0x0a, 0b00111011);

  // P1_R11, OC protection, keep default

  // P1_R12, HPL Routing
  // no DAC reconstruction filters (D3=0)
  // IN1L is routed to HPL (D2=1)
  // MixerL is routed to HPL (D1=1)
  // MixerR is not routed to HPL (D0=0)
  sendData(AUDIO_I2C_ADDR, 0x0c, HP_OUTPUT);

  // P1_R13, HPR Routing
  // no DAC filters (D4=D3=0)
  // IN1R is routed to HPR (D2=1)
  // MixerR is routed to HPR (D1=1)
  // HPL is not routed to HPR (D0=0)
  sendData(AUDIO_I2C_ADDR, 0x0d, HP_OUTPUT);

  // P1_R14, LOL Routing
  // no DAC reconstruction filter (D4=D3=0)
  // MixerL is routed to LOL (D1=1)
  // LOR is not routed to LOL (D0=0)
  sendData(AUDIO_I2C_ADDR, 0x0e, 0b00000010);

  // P1_R15, LOR Routing
  // no DAC reconstruction filter (D3=0)
  // MixerR is routed to LOR (D1=1)
  sendData(AUDIO_I2C_ADDR, 0x0f, 0b00000010);

  // P1_R18, LOL Driver Gain
  // P1_R19, LOR Driver Gain
  // LO is not muted (D6=0)
  // LO Gain (D5~D0)
  sendData(AUDIO_I2C_ADDR, 0x12, LO_GAIN);
  sendData(AUDIO_I2C_ADDR, 0x13, LO_GAIN);

  // P1_R20, HP Startup Control, keep default
  sendData(AUDIO_I2C_ADDR, 0x14, 0b00101001);

  // P1_R22, IN1L->HPL Volume Control
  // P1_R23, IN1R->HPR Volume Control
  // keep default (+0dB)

  // P1_R24, MixerL Volume Control
  // P1_R25, MixerR Volume Control
  sendData(AUDIO_I2C_ADDR, 0x16, HP_ANALOG_GAIN);
  sendData(AUDIO_I2C_ADDR, 0x17, HP_ANALOG_GAIN);

  // P1_R52, Left MICPGA Positive Routing
  // IN1L-> Left MICPGA with 20k resistance (D7~D6=10)
  sendData(AUDIO_I2C_ADDR, 0x34, 0b10000000);

  // P1_R54, Left MICPGA Negative Routing
  // CM->Left MICPGA with 20k resistance (D7~D6=10)
  sendData(AUDIO_I2C_ADDR, 0x36, 0b10000000);
  
  // P1_R55, Right MICPGA Positive Routing
  // IN1R->Right MICPGA with 20k resistance (D7~D6=10)
  sendData(AUDIO_I2C_ADDR, 0x37, 0b10000000);

  // P1_R57, Right MICPGA Negative Routing
  // CM->Right MICPGA with 20k resistance (D7~D6=10)
  sendData(AUDIO_I2C_ADDR, 0x39, 0b10000000);

  // P1_R58, Floating Input Config
  // weakly connect unused pins to CM (D7~D2)
  sendData(AUDIO_I2C_ADDR, 0x3a, 0b00111100);

  // P1_R59, Left MICPGA Volume
  // P1_R60, Right MICPGA Volume
  // volume control (D6~D0)
  sendData(AUDIO_I2C_ADDR, 0x3b, MICPGA_GAIN);
  sendData(AUDIO_I2C_ADDR, 0x3c, MICPGA_GAIN);

  // P1_R61, ADC PowerTune, keep default

  // P1_R16, HPL Driver Gain
  // P1_R17, HPR Driver Gain
  // HP is not muted (D6=0)
  // HP Gain (D5~D0)
  sendData(AUDIO_I2C_ADDR, 0x10, HP_GAIN);
  sendData(AUDIO_I2C_ADDR, 0x11, HP_GAIN);
}

void sendData(uint8_t addr, uint8_t reg, uint8_t value) {
  uint8_t buf[2] = {reg, value};
  int result = i2c_write_timeout_us(i2c0, addr, buf, sizeof(buf), false, 50000);
}

void beep(int consTime) {
  tone(17, 1000);
  delay(consTime);
  noTone(17);
}

void setup() {
  // ----- TLV320AIC3204 -----
  // RST pin
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  delay(100);
  
  // control interface (I2C0)
  i2c_init(i2c0, 100000);
  gpio_set_function(SDA, GPIO_FUNC_I2C);
  gpio_set_function(SCL, GPIO_FUNC_I2C);
  gpio_pull_up(SDA);
  gpio_pull_up(SCL);

  // I2S
  audio.setBCLK(BCLK); // LRCLK = BCLK + 1 = 8
  audio.setDOUT(DOUT);
  audio.setDIN(DIN);
  audio.setDATA(DIN);
  audio.setMCLK(MCLK);
  audio.setBitsPerSample(16);
  audio.setFrequency(48000);
  audio.setSysClk(48000); // change system clock to meet mclk
  audio.setMCLKmult(64); // MCLK = 48k * 64 = 3.072MHz
  audio.setBuffers(I2S_BUFFER_NUM , I2S_BUFFER_SIZE);
  audio.begin();
  // init audio chip
  audioInit();

}

unsigned long lastFlushTime = 0;

void setup1() {
  // SD Card
  SPI1.setRX(SD_MISO); // 24
  SPI1.setTX(SD_MOSI); // 27
  SPI1.setSCK(SD_SCK); // 26
  SPI1.setCS(SD_CS); // 29
  pinMode(SD_CD, INPUT); // 25
  while (true) {
    if (!digitalRead(SD_CD)) { // insert a card
      delay(1000); // waiting for card running
      SDFS.setConfig(SDFSConfig(SD_CS, SD_SCK_MHZ(50), SPI1));
      if(SDFS.begin()) beep(500);
      break;
    }
  }
  // create a new file ready to be wrote
  createNewFile();
}

wavHeader fileHeader;
File waveFile;

// SDFS has a 512B write buffer, and file header is only 44B
// the file won't be really written until another data chunk arrived
void createNewFile() {
  for (int index=0; index<MAX_FILE_SERIAL; index++) {
    char fileName[10];
    sprintf(fileName, "%d.wav", index);
    if (!SDFS.exists(fileName)) {
      while(true) {
        if (BOOTSEL) {
          while(BOOTSEL);
          waveFile = SDFS.open(fileName, "w+");
          if(!waveFile) {
            beep(50); delay(50); beep(50); delay(50); beep(50);
          }
          // write file header
          waveFile.write((uint8_t*)&fileHeader, sizeof(wavHeader));
          lastFlushTime = millis();
          beep(100);
          break;
        }
      }
      break;
    }
  }
}


void loop() {

}

__attribute__((aligned(4))) uint8_t pcmBuffer[AUDIO_TRANSMIT_THRESHOLD];

void loop1() {

  if (audio.available() >= AUDIO_TRANSMIT_THRESHOLD) { 
    audio.read(pcmBuffer, AUDIO_TRANSMIT_THRESHOLD);
    updateFile();
  }

  if (millis() - lastFlushTime >= SD_FLUSH_SEGMENT) {
    waveFile.flush();
    lastFlushTime = millis();
  }

  // stop recording and save file
  if (BOOTSEL) {
    while(BOOTSEL);
    // overwrite the file header with newer file size
    waveFile.seek(0, SeekSet);
    waveFile.write((uint8_t*)&fileHeader, sizeof(wavHeader));
  
    waveFile.close();
    beep(500);
    // prepare for the next file
    createNewFile();
  }
}

void updateFile() {
  waveFile.write(pcmBuffer, AUDIO_TRANSMIT_THRESHOLD);
  // update file size & data chunk size;
  fileHeader.chunkSize += AUDIO_TRANSMIT_THRESHOLD;
  fileHeader.subChunk2Size += AUDIO_TRANSMIT_THRESHOLD;
}