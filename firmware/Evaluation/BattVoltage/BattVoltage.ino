#define ADC A2
#define ADC_EN 16

#include <SDFS.h>
#include <SPI.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"

#define recordSegment 1 // minutes

File Log;
unsigned long lastRecordTime = 0;
unsigned int runningTime = 0; // minutes

void beep(int consTime) {
  tone(17, 1000);
  delay(consTime);
  noTone(17);
}

void setup() {
  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);
  analogReadResolution(12);

  SPI1.setRX(24); // 24
  SPI1.setTX(27); // 27
  SPI1.setSCK(26); // 26
  SPI1.setCS(29); // 29
  SDFS.setConfig(SDFSConfig(29, SD_SCK_MHZ(50), SPI1));
  SDFS.begin();
  delay(1000);
  Log = SDFS.open("/log.csv", "w+");
  if(!Log) beep(500);
  lastRecordTime = millis();
  writeNewLine();

}

void writeNewLine() {
  float voltage = ((float)analogRead(ADC))/4095*3.3*3/2;
  char buffer[100];
  sprintf(buffer, "%d, %.2d:%.2d, %.2f\n", runningTime, runningTime/60, runningTime%60, voltage);
  Log.write(buffer, strlen(buffer));
  Log.flush();
}

void loop() {
  if (BOOTSEL) beep(100);
  if ((millis()-lastRecordTime)/(recordSegment*60*1000)) {
    runningTime += 1;
    writeNewLine();
    lastRecordTime = millis();
  }
}
