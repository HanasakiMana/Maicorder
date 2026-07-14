#include "RTC.h"
#include "I2C.h"
#include "ADC.h"
#include "Battery.h"
#include "RGB.h"
#include "SDCard.h"

#include "hardware/powman.h"


RTC rtc;
ADC adc;
Battery batt;
RGB led;
SDCard card;

// BOOTSEL cannot be set as an interrupt pin
// so It's a good idea to use the second core to check it
volatile bool isBootselPressed = false;
volatile bool isLowBattery = false;
uint32_t lastBatteryTestTime = 0;
uint32_t lastButtonTriggeredTime = 0;
void loop1() {
  delay(1000/BOOTSEL_CHECK_FREQ);
  if (BOOTSEL && millis() - lastButtonTriggeredTime >= BTN_NO_RESPONSE_TIME) {
    isBootselPressed = true; // reset process will be handled by Core0
    lastButtonTriggeredTime = millis();
  }
  if (millis() - lastBatteryTestTime >= BATTERY_VOLTAGE_TEST_SEGMENT) {
    float battVoltage = batt.readVoltage();
    if (battVoltage <= LOW_BATTERY_VOLTAGE) isLowBattery = true;
    lastBatteryTestTime = millis();
  }
}


void usbAndCharging() {
  // Open serial port for time setting
  Serial.begin();
  while (true) {
    delay(1000); // set refresh rate to 1s
    // Serial.printf("CPU Freq: %.2f MHz\n", (float)rp2040.f_cpu()/1000000.0f);
    // rtc setting
    time_t timeStamp = 3393936000; // 2077-07-07
    char buffer[21];
    if(Serial.available()) { // revieve time stamp from serial
      Serial.readBytesUntil('\n', buffer, sizeof(buffer));
      if (buffer[0] == 'T') {
        timeStamp = atoll(&buffer[1]);
        rtc.setTime(timeStamp);
        rtc.updateSysTime(timeStamp, TIMEZONE_OFFSET);
        led.blink(0x00ffff, 100, 3); // blink to show time has been set
      }
    }
    rtc.getTime();
    // charging status check
    if (batt.isFull()) led.setColor(0x00ff00); // full
    else if (batt.isCharging()) led.setColor(0xffff00); // charging
    else led.setColor(0xff0000); // battery temp is too high/low
    if(BOOTSEL) break; // exit if BOOTSEL is pressed
  }
}


void setup() {
  // initialize necessary components
  I2C::init();
  batt.init();
  rtc.init();
  led.init();
#ifdef DEBUG_NORMAL_MODE
  Serial.begin();
#endif
  
  // Charging and USB mode
#ifdef DEBUG_NORMAL_MODE
  if (false)
#endif
#ifndef DEBUG_NORMAL_MODE
  if (batt.isVBusConnected())
#endif
     usbAndCharging(); // will start a infinite loop internally
  
  // check if battery is low
  batt.readVoltage(); // dummy read, idk why but the first read will get wrong voltage
  float voltage = batt.readVoltage();
#ifdef DEBUG_NORMAL_MODE
    delay(3000); // waiting for virtual serial port to be connected
    Serial.printf("Battery voltage: %.2fV\n", voltage);
#endif
  if (voltage <= LOW_BATTERY_VOLTAGE) {
    led.blink(0xff0000, 500, 3); // blink red to show low battery
    powman_set_power_state(POWMAN_POWER_STATE_NONE); // enter deep sleep mode
  }

  // Normal Mode
  // update system time
  rtc.updateSysTime(rtc.getTime(), TIMEZONE_OFFSET);
  // initialize ADC
  adc.init();
  // initialize SD card
  card.init();
  while (!card.isInserted()) {
    led.blink(0xff0000, 500, 1); // no card
  }
  delay(1000); // wait for card to be initialized
  if(!card.begin()) {
    led.setColor(0xff0000); // card or file system is not supported
    while (true); // block here
  } else {
    led.setColor(0x00ffff); // ready to record
  }
}


void setup1() {}

time_t getTime() {
  return rtc.getTime() + 3600000 * TIMEZONE_OFFSET; // 60s * 60min * 1000ms
}

bool blink = false;
bool isSuppressed = false;
int8_t suppressFreeze = 0;

void loop() {
  if(isBootselPressed) { // start recording
    isBootselPressed = false; // reset button status
    SDFS.setTimeCallback(getTime);
    card.createNewFile(rtc.getTime());
    adc.clearBuffer();
    // loop for sampling and writing to sd card
    while (true) {
      // update audio buffer and calculate average loudness
      float peakdBFS = 0.0f;
      peakdBFS = adc.updateBuffer(card.pcmBuffer, sizeof(card.pcmBuffer));
#ifdef DEBUG_NORMAL_MODE
      Serial.printf("Peak: %.2fdBFS\n", peakdBFS);
#endif
      // suppress if audio is too loud
      if (suppressFreeze >  0) suppressFreeze -= 1;
      if (peakdBFS >= PEAK_SUPPRESS_THRESHOLD && suppressFreeze == 0) {
        adc.changeGain((float)SUPPRESS_DEPTH);
        suppressFreeze = SUPPRESS_FREEZE_SEGMENT;
        isSuppressed = true;
#ifdef DEBUG_NORMAL_MODE
        Serial.printf("SUPPRESS TRIGGERED, change gain to %ddB\n", adc.currentGain/2);
#endif
      }
      card.updateFile();
      if (blink) {
        if (isLowBattery) led.setColor(0xffff00); // low bettery
        else if (isSuppressed && suppressFreeze > 0) led.setColor(0xff0000); // suppression triggered
        else led.setColor(0x00ffff); // normal
        blink = false;
      } else {
        if (isSuppressed && suppressFreeze > 0) led.setColor(0xff0000); // stay red
        else led.setColor(0); // blink normally
        blink = true;
      }
      // stop recording
      if (isBootselPressed) { 
        isBootselPressed = false;
        card.close();
        if (isLowBattery) {
          led.blink(0xffff00, 100, 5);
          led.setColor(0xffff00);
        }
        else led.setColor(0x00ffff);
        break;
      }
    }

  }
}
