#include "pcm1863.h"
#include "SDCard.h"
#include "Battery.h"
#include "RGB.h"
#include "pinDef.h"
#include "beep.h"


Battery batt;
PCM1863 codec;
SDCard card;
RGB led;

void setup() {
  led.init();
  batt.init();
  card.init();
  led.setColor(0x000000);
  
  while(!card.isInserted());
  card.begin();
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

void loop() {
  if(BOOTSEL) {
    beep(500);
    card.close();
  }
  codec.updateBuffer(card.pcmBuffer, sizeof(card.pcmBuffer));
  card.updateFile();
}

