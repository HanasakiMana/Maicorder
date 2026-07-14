#include "pcm1863.h"
#include "SDCard.h"
#include "Battery.h"
#include "RGB.h"
#include "pinDef.h"
#include "beep.h"

#include <Wire.h>
#include "arm_math.h"

Battery batt;
PCM1863 codec;
SDCard card;
RGB led;

void i2cInit() {
    i2c_init(i2c0, I2C_CLK);
    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);
}

void setup() {
  
  Serial.begin(115200);
  pinMode(ALDO_EN, OUTPUT);
  digitalWrite(ALDO_EN, HIGH);
  led.init();
  batt.init();
  card.init();
  /*if (batt.isVBusConnected()) {
    while (true) {
        if (batt.isFull()) led.setColor(0x00ff00);
        else {
          if (batt.isCharging()) led.setColor(0xff0000);
          else led.setColor(0x0000ff);
        }
      Serial.printf("isFull: %d, isCharging: %d\n", batt.isFull(), batt.isCharging());
      delay(1000);
    }
  }*/

  led.setColor(0x00ff00);
  
  while(!card.isInserted());
  delay(1000); // waiting initialization of card
  if(!card.begin()) led.setColor(0xff0000);
  else led.setColor(0x00ffff);
  // waiting for pressing record button
  while(!BOOTSEL);
  delay(100);
  while(BOOTSEL);
  beep(100);
  card.createNewFile();
}

void setup1() {
  codec.init();
}

q15_t absBuffer[AUDIO_TRANSMIT_THRESHOLD/8] = {0};

bool blink = false;

void loop() {
  if(BOOTSEL) {
    led.setColor(0xff0000);
    // beep(500);
    card.close();
    delay(1000);
  }
  codec.updateBuffer(card.pcmBuffer, sizeof(card.pcmBuffer));
  q15_t currentRMS = 0;
  float delta = 0.0f;
  int8_t deltaGain2x = 0;
  
  arm_rms_q15((q15_t *)card.pcmBuffer, sizeof(card.pcmBuffer), &currentRMS);
  // delta>0, current is lower than target
  // delta<0, current is higher than target
  /*delta = TARGET_AVG_LOUDNESS_DBFS - AGC().rms2dbfs(currentRMS);
  deltaGain2x = (int8_t) (delta*2);
  if (deltaGain2x + codec.pgaGain > (2 * AGC_GAIN_MAX)) codec.pgaGain = (2 * AGC_GAIN_MAX);
  else {
    if (deltaGain2x + codec.pgaGain < (2 * AGC_GAIN_MIN)) codec.pgaGain = (2 * AGC_GAIN_MIN);
    else {
      codec.pgaGain = deltaGain2x + codec.pgaGain;
    }
  }
  codec.setPgaGain(codec.pgaGain);*/
  

  
  Serial.printf("Clipping Accured: %d\n", codec.i2cRead(0x61)>>4&1);
  codec.i2cWrite(0x60, 0x01);
  codec.i2cWrite(0x60, 0x11);
  Serial.printf("AVR: %.1fdBFS\n", AGC().rms2dbfs(currentRMS));
  // Serial.printf("Used/Free/Total Heap: %dKB, %dKB, %dKB\n", rp2040.getUsedHeap()/1024, rp2040.getFreeHeap()/1024, rp2040.getTotalHeap()/1024);
  unsigned long startWrite = millis();
  card.updateFile();
  if(blink) {
    led.setColor(0x00ffff);
    blink = false;
  } else {
    led.setColor(0x000000);
    blink = true;
  }
  Serial.printf("Write speed: %.2fMB/s\n", 0.0625f/((float)(millis() - startWrite)/1000.0f));
}

